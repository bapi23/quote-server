#include "CoinbaseTransportLayer.hpp"
#include "MessageHandler.hpp"
#include "MarkedChangeListener.hpp"

#include <nlohmann/json.hpp>


using json = nlohmann::json;

class CoinbaseMessageHandler: public MessageHandler{
public:
    CoinbaseMessageHandler(MarketChangeListener* listener): m_listener(listener)
    {
        m_listener = listener;
    }

private:
    void onHandleMessage(const std::string& msg){
        //create coinbase market change
        m_listener->onMarketChange(generateMarketChange(msg));
    }

    std::unique_ptr<MarketChange> generateMarketChange(const std::string& msgPayload){
    }

    MarketChangeListener* m_listener;
};