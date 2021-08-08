#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "RestTransport.hpp"
#include "WebsocketTransport.hpp"
#include "FeedClient.hpp"
#include "CoinbaseFeedListener.hpp"
#include "FeedClient.hpp"


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
        feedTransport.subscribe("coinbaseFeed", this);
        feedTransport.connect("wss://ws-feed.pro.coinbase.com");
    }

    ~CoinbaseFeedClient(){
        feedTransport.unsubscribe("coinbaseFeed");
    }

    void onMessageReceived(const std::string& message){
        //std::cout << "received message" << message;
    }

    // void run(){
    //     WebsocketTransport transport;
    //     transport.connect("wss://ws-feed.pro.coinbase.com");
    //     sleep(1);
    //     transport.send(subscribe_test.dump());
    //     while(isRunning){
    //         sleep(1);
    //     }
    // }

    void subscribe(const std::string& productId, ProductChangeListener* listener) override{
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
    bool isRunning = true;
};