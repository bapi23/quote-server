#include <vector>
#include <algorithm>

#include "RestTransport.hpp"
#include "WebsocketTransport.hpp"
#include "MessageHandler.hpp"


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


class CoinbaseTransportProxy: public MessageReceiver{
private:
    CoinbaseTransport transport;
    std::queue<std::string> msg_queue;
};


class CoinbaseTransport: public MessageReceiver{
public:
    CoinbaseTransport(){
        feedTransport.subscribe(this);
        feedTransport.connect("wss://ws-feed.pro.coinbase.com");
    }

    ~CoinbaseTransport(){
        feedTransport.unsubscribe(this);
    }

    void subscribe(const std::string& productId, MessageHandler* handler){
        if(prodIdToHandlers.find(productId) == std::end(prodIdToHandlers)){
            prodIdToHandlers[productId] = std::vector<MessageHandler*>();
            feedTransport.send(generateSubscribeMessage(productId));
        }
        prodIdToHandlers[productId].push_back(handler);
    }

    void unsubscribe(const std::string& productId, MessageHandler* handler){
        if(prodIdToHandlers.find(productId) == std::end(prodIdToHandlers)){
            std::cout << "Can't find " << productId << " to unsubscribe";
            return;
        }

        auto& vec = prodIdToHandlers.at(productId);
        auto it = std::find_if(std::begin(vec), std::end(vec));
        vec.erase(it);
        
        if(vec.empty()){
            feedTransport.send(generateUnsubscribeMessage(productId));
        }
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

    void onMessageReceived(const std::string msg){
        const auto json_msg = json::parse(msg);
        if(json_msg["type"] == "subscriptions"){
            //TODO unsubscribed
        }
    }

    std::unordered_map<std::string, std::vector<MessageHandler*>> prodIdToHandlers;
    std::unordered_map<std::string> unsibscribe_queue;
    std::unordered_map<std::string, std::vector<MessageHandler*>> subscribe_queue;
    WebsocketTransport feedTransport;
    RestTransport restTransport;
    mutable std::mutex handlers_mtx;
};