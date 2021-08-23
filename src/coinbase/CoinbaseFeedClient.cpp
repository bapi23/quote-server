
#include <vector>
#include <algorithm>
#include "nlohmann/json.hpp"
#include <boost/numeric/conversion/cast.hpp>

#include "RestTransport.hpp"
#include "WebsocketTransport.hpp"
#include "FeedClient.hpp"
#include "coinbase/CoinbaseFeedProduct.hpp"
#include "coinbase/CoinbaseFeedClient.hpp"

namespace {

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

}

CoinbaseFeedClient::CoinbaseFeedClient(): m_stamps(2000){}

CoinbaseFeedClient::~CoinbaseFeedClient(){
    feedTransport.unsubscribe("coinbaseFeed");
}

void CoinbaseFeedClient::onMessageReceived(const std::string& message){
    std::lock_guard<std::mutex> lg(mutexFeedData);
    try{
        auto jmsg = json::parse(message);
        if(!jmsg.contains("type")){
            std::cout << "Received message without type" << std::endl;
            return;
        }

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        m_stamps.push_back(begin);

        if(m_stamps.size() > 0){
            auto last = m_stamps.back();
            auto first = m_stamps.front();
            auto difference_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last - first).count();
            auto messages_per_second = (boost::numeric_cast<float>(difference_ms)/boost::numeric_cast<float>(m_stamps.size())) * 1000;
            std::cout << "Feed frequency = " << messages_per_second << " messages/s from the last " << m_stamps.size() << " messages" << std::endl;
            //send metrics
        }

        if(jmsg.contains("product_id")){
            auto prodId = jmsg["product_id"].get<std::string>();
            auto it = prodIdToListener.find(prodId);
            if(it == prodIdToListener.end()){
                //std::cout << "Got product id which was not subscribed to!" << std::endl;
            } else {
                it->second->onMessageReceived(jmsg);
            }
        } else if(jmsg["type"] == "subscriptions"){
            std::cout << "Received subscribe message" << jmsg << std::endl;
        } else if(jmsg["type"] == "unsubscribe"){
            std::cout << "Received unsubscribe message" << jmsg << std::endl;
        }

    } catch(nlohmann::detail::parse_error){
        std::cout << "Can't parse message: " << message;
    }

}

void CoinbaseFeedClient::subscribe(const std::string& productId, ProductChangeListener* listener) {
    std::lock_guard<std::mutex> lg(mutexFeedData);
    {
        if(!m_connected){
            feedTransport.subscribe("coinbaseFeed", this);
            feedTransport.connect("wss://ws-feed.pro.coinbase.com");
            m_connected = true;
            
        }
        auto it = prodIdToListener.find(productId);
        if(it == prodIdToListener.end()){
            prodIdToListener.emplace(
                std::make_pair(productId, std::make_unique<CoinbaseFeedProduct>(productId, listener)));
        } else {
            std::cout << "listener already registered";
        }
        std::cout << "Registering listener for prod id: " << productId << std::endl;
    }
    feedTransport.send(generateSubscribeMessage(productId));
}

void CoinbaseFeedClient::unsubscribe(const std::string& productId) {
    std::cout << "Unsubscribed " << productId << std::endl;
    feedTransport.send(generateUnsubscribeMessage(productId));
    std::lock_guard<std::mutex> lg(mutexFeedData);
    prodIdToListener.erase(productId);
}

std::string CoinbaseFeedClient::generateSubscribeMessage(const std::string& productId){
    auto message = subscribe_template;
    message["product_ids"] = {productId};
    return message.dump();
}

std::string CoinbaseFeedClient::generateUnsubscribeMessage(const std::string& productId){
    auto message = unsibscribe_template;
    message["product_ids"] = {productId};
    return message.dump();
}