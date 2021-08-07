#include <string>
#include "CoinbaseFeedMessageHandler.hpp"
#include "WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

class CoinbaseFeedListener: public MessageReceiver {
public:
    CoinbaseFeedListener(const std::string productId, ProductChangeListener* listener): 
        m_productId(productId), m_msgHandler(listener){}

    void onMessageReceived(const std::string& message) override;
        m_msgHandler.onMessageReceived(msg);
    }


private:
    std::string m_productId;
    CoinbaseFeedMessageHandler m_msgHandler;
    std::vector<std::string> m_messageQueue;
    int lastSequenceNumber;

    WebsocketTransport
};