#pragma once

#include <string>
#include <deque>
#include <sstream>
#include <nlohmann/json.hpp>


#include "CoinbaseFeedMessageHandler.hpp"
#include "WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

using json = nlohmann::json;

class CoinbaseFeedListener{
public:
    CoinbaseFeedListener(const std::string productId, ProductChangeListener* listener): 
        m_productId(productId), 
        m_msgHandler(listener, productId), 
        m_messageQueue(), 
        m_lastSequenceNumber(0)
    {
        std::cout << "[" << m_productId << "]" << "Requesting full orderbook" << std::endl;
        m_fullOrderBookRequestThread.reset(new std::thread(std::bind(&CoinbaseFeedListener::requestFullOrderBookImpl, this)));
        m_messageHandlerThread.reset(new std::thread(std::bind(&CoinbaseFeedListener::messageLoop, this)));
    }

    void requestFullOrderBook(){
        m_requestingOrderbook = true;
        if(m_fullOrderBookRequestThread && m_fullOrderBookRequestThread->joinable()){
            m_fullOrderBookRequestThread->join();
        }
        std::cout << "[" << m_productId << "]" << "Requesting full orderbook" << std::endl;
        m_fullOrderBookRequestThread.reset(new std::thread(std::bind(&CoinbaseFeedListener::requestFullOrderBookImpl, this)));
    }

    void onMessageReceived(const nlohmann::json& message) {
        std::unique_lock<std::mutex> lk(m_messagesMutex);
        unsigned long int sequence = 0;
        std::string sequenceStr = message["sequence"].get<std::string>();
        std::stringstream(sequenceStr) >> sequence;

        if(m_requestingOrderbook){
            m_messageQueue.push_back(message);
            return;
        }
        if( sequence > m_lastSequenceNumber + 1){
            //std::cout << "[" << m_productId << "]" << "Received gap between message" << m_lastSequenceNumber << " vs " << sequence << std::endl;
            m_lastSequenceNumber = 0;
            requestFullOrderBook();
        } else if (sequence <= m_lastSequenceNumber){
            //std::cout << "[" << m_productId << "]" << "Received too old message: " << sequence << " Last one:" << m_lastSequenceNumber << std::endl;
            return;
        } else {
            //std::cout << "[" << m_productId << "]" << "Received up-to-date message: " << sequence << " Last one:" << m_lastSequenceNumber << std::endl;
            m_messageQueue.push_back(message);
            m_lastSequenceNumber = sequence;
            m_messages_cv.notify_one();
        }
    }

    ~CoinbaseFeedListener(){
        m_isRunning = false;
        if(m_fullOrderBookRequestThread && m_fullOrderBookRequestThread->joinable()){
            m_fullOrderBookRequestThread->join();
        }
        if(m_messageHandlerThread && m_messageHandlerThread->joinable()){
            m_messageHandlerThread->join();
        }
    }

private:
    void requestFullOrderBookImpl(){

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        
        std::string request = "https://api.pro.coinbase.com/products/" + m_productId + "/book?level=3";
        auto fullOrderbookMessage = RestTransport::request(request);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Full Orderbook request took = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

        if(!fullOrderbookMessage.empty()){
            auto orderBookMsg = nlohmann::json::parse(fullOrderbookMessage);
            if(!orderBookMsg.contains("sequence"))
            {
                std::cout << "[" << m_productId << "]" << "INVALID ORDERBOOK RECEIVED" << std::endl;
                return;
            }

            unsigned long int orderBookSequence = 0;
            std::string sequenceStr = orderBookMsg["sequence"].get<std::string>();
            std::stringstream(sequenceStr) >> orderBookSequence;

            if(orderBookSequence == 0){
                std::cout << "[" << m_productId << "]" << "INVALID ORDERBOOK SEQUENCE" << std::endl;
                return;
            }
            
            std::lock_guard<std::mutex> lg(m_messagesMutex);
            {
                m_lastSequenceNumber = orderBookSequence;
                std::cout << "orderbook sequce:" << m_lastSequenceNumber << std::endl;
                while(!m_messageQueue.empty()){
                    auto message = m_messageQueue.front();
                    m_messageQueue.pop_front();
                    if(message.contains("sequence")){
                        unsigned long int sequence = 0;
                        std::string sequenceStr = message["sequence"].get<std::string>();
                        std::stringstream(sequenceStr) >> sequence;
                        //std::cout << "Msg sequence" << sequence << std::endl;
                        if(sequence > orderBookSequence){
                            m_lastSequenceNumber = sequence;
                            //std::cout << "Replacing Msg sequence to " << sequence << std::endl;
                            orderBookMsg["to_merge"].push_back(message);
                        }
                    } else {
                        std::cout << "[" << m_productId << "]" << "Received message without sequence!" << std::endl;
                    }
                }
                m_messageQueue.push_front(orderBookMsg);
                m_requestingOrderbook = false;
                m_messages_cv.notify_one();
            }

        } else {
            std::cout << "[" << m_productId << "]" << "RECEIVED EMPTY FULL ORDERBOOK MESSAGE!" << std::endl << std::flush;
        }
    }

    void messageLoop(){
        while(m_isRunning){
            using namespace std::chrono_literals;
            std::deque<nlohmann::json> messages;
            {
                std::unique_lock<std::mutex> lk(m_messagesMutex);
                m_messages_cv.wait_for(lk, 1000ms, [this]{return !m_messageQueue.empty();});
                if(!m_requestingOrderbook){
                    messages = m_messageQueue;
                    m_messageQueue.clear();
                }
            }
            for(const auto& msg: messages){
                //std::cout << "Handling for " << m_productId << std::endl;
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
    long unsigned m_lastSequenceNumber;
    std::mutex m_messagesMutex;
    std::condition_variable m_messages_cv;
    std::atomic<bool> m_isRunning{true};
    std::atomic<bool> m_requestingOrderbook{true};
    std::unique_ptr<std::thread> m_messageHandlerThread;
    std::unique_ptr<std::thread> m_fullOrderBookRequestThread;
    
};