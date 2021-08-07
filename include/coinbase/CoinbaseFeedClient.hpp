#include <vector>
#include <algorithm>

#include "RestTransport.hpp"
#include "WebsocketTransport.hpp"
#include "MessageHandler.hpp"
#include "FeedClient.hpp"
#include "CoinbaseFeedListener.hpp"


auto subscribe_template = json::parse(R"({
    "type": "subscribe",
    "product_ids": [],
    "channels": [
        "full",
        "heartbeat",
        "ticker"
    ]
})");

auto unsibscribe_template = json::parse(R"({
    "type": "unsubscribe",
    "product_ids": [],
    "channels": ["ticker", "full", "heartbeat"]
})");


class CoinbaseFeedClient: public FeedClient, MessageReceiver{
public:
    CoinbaseFeedClient(){
        feedTransport.subscribe(this);
        feedTransport.connect("wss://ws-feed.pro.coinbase.com");
    }

    ~CoinbaseFeedClient(){
        feedTransport.unsubscribe(this);
    }

    void subscribe(const std::string& productId, ProductChangeListener* listener) override{
        auto it = prodIdToListener.find(productId);
        if(it == prodIdToListener.end()){
            prodIdToListener[productId] = CoinbaseFeedListener(productId, listener);
        } else {
            if(*it != listener) {
                std::cout << "Different listener already registered!"
            } else {
                std::cout << "listener already registered";
            }
        }
    }

    void unsubscribe(const std::string& productId, ProductChangeListener* listener) override{
        
    }

private:
    std::string generateSubscribeMessage(const std::string& productId){
        auto message = subscribe_template;
        message["product_ids"] = {productId};
        return message.dump();
    }

    std::string generateUnsubscribeMessage(const std::string& productId){
        auto message = subscribe_template;
        message["product_ids"] = {productId};
        return message.dump();
    }

    std::unordered_map<std::string, CoinbaseFeedListener> prodIdToListener;
};