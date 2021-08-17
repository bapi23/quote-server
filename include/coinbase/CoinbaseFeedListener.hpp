#include <string>
#include <deque>
#include <nlohmann/json.hpp>


#include "CoinbaseFeedMessageHandler.hpp"
#include "transport/WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

using json = nlohmann::json;

class CoinbaseFeedListener: public MessageReceiver {
public:
    CoinbaseFeedListener(const std::string productId, std::weak_ptr<ProductChangeListener> listener): 
        m_productId(productId), 
        m_msgHandler(listener), 
        m_messageQueue(), 
        m_lastSequenceNumber(0),
        m_messageHandlerThread(std::bind(&CoinbaseFeedListener::notifyHandler, this))
    {
        requestFullOrderBook();
    }

    void requestFullOrderBook(){
        std::cout << "Requesting full orderbook" << std::endl;
        if(m_fullOrderBookRequestThread && m_fullOrderBookRequestThread->joinable()){
            m_fullOrderBookRequestThread->join();
        }
        m_fullOrderBookRequestThread.reset(new std::thread(std::bind(&CoinbaseFeedListener::requestFullOrderBookImpl, this)));
    }

    void requestFullOrderBookImpl(){
        std::string request = "https://api.pro.coinbase.com/products/" + m_productId + "/book?level=3";
        auto fullOrderbookMessage = RestTransport::request(request);

        if(!fullOrderbookMessage.empty()){
            auto jmsg = nlohmann::json::parse(fullOrderbookMessage);
            const auto orderBookSequence = jmsg["sequence"];
            std::lock_guard<std::mutex> lg(m_messagesMutex);
            std::deque<std::string> relevantMessages;
            while(!m_messageQueue.empty()){
                auto message = m_messageQueue.front();
                m_messageQueue.pop_front();
                auto parsed = nlohmann::json::parse(message);
                if(parsed.contains("sequence")){
                    if(parsed["sequence"] > orderBookSequence){
                        relevantMessages.push_front(message);
                    }
                } else {
                    relevantMessages.push_front(message);
                }
            }
            m_messageQueue.push_front(fullOrderbookMessage);
            m_messageQueue = relevantMessages;

        }
        m_messages_cv.notify_one();
    }

    void onMessageReceived(const std::string& message) override {
        auto jmessage = json::parse(message);

        int sequence = jmessage["sequence"].get<int>();
        if (m_lastSequenceNumber != 0){
            if( sequence > m_lastSequenceNumber + 1){
                // we have a gap between sequence numbers - reinitializing everything:
                m_lastSequenceNumber = 0;
                {
                    std::unique_lock<std::mutex> lk(m_messagesMutex);
                    m_messageQueue.push_back(message);
                }
                requestFullOrderBook();
            }
        }
        
        {
            std::unique_lock<std::mutex> lk(m_messagesMutex);
            m_messageQueue.push_back(message);
            m_lastSequenceNumber = sequence;
        }
        m_messages_cv.notify_one();
    }

    void notifyHandler(){
        while(m_isRunning){
            std::deque<std::string> messages;
            {
                std::unique_lock<std::mutex> lk(m_messagesMutex);
                m_messages_cv.wait(lk, [this]{return !m_messageQueue.empty();});
                messages = m_messageQueue;
            }
            for(const auto& msg: messages){
                m_msgHandler.onMessageReceived(msg);
            }
        }
    }

    CoinbaseFeedListener(CoinbaseFeedListener&& other) = delete;
    CoinbaseFeedListener& operator=(CoinbaseFeedListener&& other) = delete;
    CoinbaseFeedListener(CoinbaseFeedListener&) = delete;
    CoinbaseFeedListener& operator=(CoinbaseFeedListener&) = delete;

    ~CoinbaseFeedListener(){
        m_fullOrderBookRequestThread->join();
        m_isRunning.store(false);
        m_messageHandlerThread.join();
    }

private:
    std::string m_productId;
    CoinbaseFeedMessageHandler m_msgHandler;
    std::deque<std::string> m_messageQueue;
    int m_lastSequenceNumber;
    std::mutex m_messagesMutex;
    std::condition_variable m_messages_cv;
    std::atomic<bool> m_isRunning{true};
    std::thread m_messageHandlerThread;
    std::unique_ptr<std::thread> m_fullOrderBookRequestThread;
};