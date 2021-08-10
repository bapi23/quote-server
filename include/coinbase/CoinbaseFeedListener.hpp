#include <string>
#include <deque>
#include <nlohmann/json.hpp>


#include "CoinbaseFeedMessageHandler.hpp"
#include "WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

using json = nlohmann::json;

class CoinbaseFeedListener: public MessageReceiver {
public:
    CoinbaseFeedListener(const std::string productId, ProductChangeListener* listener): 
        m_productId(productId), 
        m_msgHandler(listener), 
        m_messageQueue(), 
        m_lastSequenceNumber(0)
    {
        std::thread(std::bind(&CoinbaseFeedListener::notifyHandler, this)).detach();
        requestFullOrderbook();
    }

    void requestFullOrderbook(){
        std::thread(std::bind(&CoinbaseFeedListener::requestFullOrderbookImpl, this)).detach();
    }

    void requestFullOrderbookImpl(){
        std::string request = "https://api.pro.coinbase.com/products/" + m_productId + "/book?level=3";
        auto initialMessage = RestTransport::request(request);
        {
            std::lock_guard<std::mutex> lg(m_messagesMutex);
            m_messageQueue.push_front(initialMessage);
        }
        m_messages_cv.notify_one();
    }

    void onMessageReceived(const std::string& message) override {
        auto jmessage = json::parse(message);

        int sequence = jmessage["sequence"].get<int>();
        if (m_lastSequenceNumber != 0){
            if( sequence > m_lastSequenceNumber+1){
                // we have a gap between sequence numbers - reinitializing everything:
                m_lastSequenceNumber = 0;
                {
                    std::unique_lock<std::mutex> lk(m_messagesMutex);
                    m_messageQueue.push_back(message);
                }
                requestFullOrderbook();
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
        while(true){
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

private:
    std::string m_productId;
    CoinbaseFeedMessageHandler m_msgHandler;
    std::deque<std::string> m_messageQueue;
    int m_lastSequenceNumber;
    std::mutex m_messagesMutex;
    std::condition_variable m_messages_cv;
};