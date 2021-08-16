#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "transport/RestTransport.hpp"
#include "transport/WebsocketTransport.hpp"
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

    ~CoinbaseFeedClient(){
        feedTransport.unsubscribe("coinbaseFeed");
    }

    void onMessageReceived(const std::string& message){
        try{
            auto jmsg = json::parse(message);
            if(jmsg.contains("product_id")){
                auto prodId = jmsg["product_id"].get<std::string>();
                auto it = prodIdToListener.find(prodId);
                if(it == prodIdToListener.end()){
                    std::cout << "ERROR: got product id which was not subscribed to!" << std::endl;
                } else {
                    it->second->onMessageReceived(message);
                }
            } else if(jmsg.contains("type") && jmsg["type"].contains("subscriptions")){
                std::cout << "Received subscribe message" << std::endl;
            }

        } catch(nlohmann::detail::parse_error){
            std::cout << "Can't parse message: " << message;
        }

    }

    void subscribe(const std::string& productId, std::weak_ptr<ProductChangeListener> listener) override{
        if(!m_connected){
            feedTransport.subscribe("coinbaseFeed", this);
            feedTransport.connect("wss://ws-feed.pro.coinbase.com");
            m_connected = true;
            
        }
        auto it = prodIdToListener.find(productId);
        if(it == prodIdToListener.end()){
            prodIdToListener.emplace(
                std::make_pair(productId, std::make_unique<CoinbaseFeedListener>(productId, listener)));
        } else {
            std::cout << "listener already registered";
        }
        std::cout << "Registering listener for prod id: " << productId << std::endl;
        feedTransport.send(generateSubscribeMessage(productId));
    }

    void unsubscribe(const std::string& productId) override{
        feedTransport.send(generateUnsubscribeMessage(productId));
        prodIdToListener.erase(productId);
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

    std::unordered_map<std::string, std::unique_ptr<CoinbaseFeedListener>> prodIdToListener;
    WebsocketTransport feedTransport;
    bool m_connected = false;
};