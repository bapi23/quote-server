#pragma once

#include <string>
#include <deque>
#include <nlohmann/json.hpp>


#include "CoinbaseFeedMessageHandler.hpp"
#include "transport/WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

using json = nlohmann::json;

class CoinbaseFeedListener{
public:
    CoinbaseFeedListener(const std::string productId, ProductChangeListener* listener): 
        m_productId(productId), 
        m_msgHandler(listener, productId), 
        m_messageQueue(), 
        m_lastSequenceNumber(0),
        m_messageHandlerThread(std::bind(&CoinbaseFeedListener::messageLoop, this))
    {
        requestFullOrderBook();
    }

    void requestFullOrderBook(){
        std::cout << "Requesting full orderbook" << std::endl;
        if(m_isRunning){
            if(m_fullOrderBookRequestThread && m_fullOrderBookRequestThread->joinable()){
                m_fullOrderBookRequestThread->join();
            }
            m_fullOrderBookRequestThread.reset(new std::thread(std::bind(&CoinbaseFeedListener::requestFullOrderBookImpl, this)));
        }
    }

    void onMessageReceived(const nlohmann::json& message) {
        int sequence = message["sequence"].get<int>();
        bool isThereAGap = false;
        {
            std::unique_lock<std::mutex> lk(m_messagesMutex);
            if (m_lastSequenceNumber != 0){
                if( sequence > m_lastSequenceNumber + 1){
                    isThereAGap = true;
                    // we have a gap between sequence numbers - reinitializing everything:
                    m_lastSequenceNumber = 0;
                }
            }
        }
        if (isThereAGap){
            requestFullOrderBook();
        }
        
        {
            std::unique_lock<std::mutex> lk(m_messagesMutex);
            m_messageQueue.push_back(message);
            m_lastSequenceNumber = sequence;
        }
        m_messages_cv.notify_one();
    }

    ~CoinbaseFeedListener(){
        m_isRunning.store(false);
        m_fullOrderBookRequestThread->join();
        m_messageHandlerThread.join();
        std::cout << "is joinable <<" << m_fullOrderBookRequestThread->joinable() << std::endl;
        std::cout << "is joinable <<" << m_messageHandlerThread.joinable() << std::endl;
    }

private:
    void requestFullOrderBookImpl(){
        std::string request = "https://api.pro.coinbase.com/products/" + m_productId + "/book?level=3";
        auto fullOrderbookMessage = RestTransport::request(request);

        if(!fullOrderbookMessage.empty()){
            auto orderBookMsg = nlohmann::json::parse(fullOrderbookMessage);
            const auto orderBookSequence = orderBookMsg["sequence"];
            std::lock_guard<std::mutex> lg(m_messagesMutex);
            m_lastSequenceNumber = orderBookSequence;
            while(!m_messageQueue.empty()){
                auto message = m_messageQueue.front();
                m_messageQueue.pop_front();
                if(message.contains("sequence")){
                    if(message["sequence"] > orderBookSequence){
                        message["to_merge"].push_back(message);
                    }
                } else {
                    std::cout << "Received message without sequence!" << std::endl;
                }
            }
            m_messageQueue.push_front(fullOrderbookMessage);
        }
        m_messages_cv.notify_one();
    }

    void messageLoop(){
        while(m_isRunning){
            std::deque<nlohmann::json> messages;
            {
                std::unique_lock<std::mutex> lk(m_messagesMutex);
                m_messages_cv.wait(lk, [this]{return !m_messageQueue.empty();});
                messages = m_messageQueue;
                m_messageQueue.clear();
            }
            for(const auto& msg: messages){
                std::cout << "Handling for " << m_productId << std::endl;
                m_msgHandler.onMessageReceived(msg);
            }
        }
    }

    CoinbaseFeedListener(CoinbaseFeedListener&& other) = delete;
    CoinbaseFeedListener& operator=(CoinbaseFeedListener&& other) = delete;
    CoinbaseFeedListener(CoinbaseFeedListener&) = delete;
    CoinbaseFeedListener& operator=(CoinbaseFeedListener&) = delete;

private:
    std::string m_productId;
    CoinbaseFeedMessageHandler m_msgHandler;
    std::deque<nlohmann::json> m_messageQueue;
    int m_lastSequenceNumber;
    std::mutex m_messagesMutex;
    std::condition_variable m_messages_cv;
    std::atomic<bool> m_isRunning{true};
    std::thread m_messageHandlerThread;
    std::unique_ptr<std::thread> m_fullOrderBookRequestThread;
};