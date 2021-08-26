
#include <vector>
#include <algorithm>
#include "nlohmann/json.hpp"
#include <boost/numeric/conversion/cast.hpp>

#include "RestTransport.hpp"
#include "WebsocketTransport.hpp"
#include "FeedClient.hpp"
#include "coinbase/CoinbaseFeedProduct.hpp"
#include "coinbase/CoinbaseFeedClient.hpp"
#include "TradeListener.hpp"
#include "coinbase/MessageToTrade.hpp"
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

bool isTradeMsg(const nlohmann::json& msg){
    if(!msg.contains("type")){
        return false;
    } else {
        if(msg["type"] == "activate" ||
           msg["type"] == "received" ||
           msg["type"] == "match") {
               return true;
           } else {
               return false;
           }
    }
}

bool isProductChangeMsg(const nlohmann::json msg){
    if(!msg.contains("type")){
        return false;
    } else {
        if(msg["type"] == "change" ||
           msg["type"] == "done" ||
           msg["type"] == "open" ||
           msg["type"] == "match" ) {
               return true;
           } else {
               return false;
           }
    }
}

}

CoinbaseFeedClient::CoinbaseFeedClient(): m_stamps(2000){}

CoinbaseFeedClient::~CoinbaseFeedClient(){
    m_feedTransport.unsubscribe("coinbaseFeed");
}

void CoinbaseFeedClient::setTradeListener(TradeListener* listener){
    m_tradeListener = listener;
}


void CoinbaseFeedClient::onMessageReceived(const std::string& message){
    std::lock_guard<std::mutex> lg(m_mutexFeedData);
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
            //std::cout << "Feed frequency = " << messages_per_second << " messages/s from the last " << m_stamps.size() << " messages" << std::endl;
            //send metrics
        }

        if(jmsg.contains("sequence")){
            auto prodId = jmsg["product_id"].get<std::string>();
            auto it = m_prodIdToListener.find(prodId);
            if(it == m_prodIdToListener.end()){
                //std::cout << "Got product id which was not subscribed to!" << std::endl;
            } else {
                it->second->onMessageReceived(jmsg);
            }
        }
        if(isTradeMsg(jmsg)){
            if(m_tradeListener){
                m_tradeListener->onTrade(MessageToTrade::getTrade(jmsg));
            }
        }
        if(jmsg["type"] == "subscriptions"){
            std::cout << "Received subscribe message" << jmsg << std::endl;
        }
        if(jmsg["type"] == "unsubscribe"){
            std::cout << "Received unsubscribe message" << jmsg << std::endl;
        }

    } catch(nlohmann::detail::parse_error){
        std::cout << "Can't parse message: " << message;
    }

}

void CoinbaseFeedClient::subscribe(const std::string& productId, ProductChangeListener* listener) {
    std::lock_guard<std::mutex> lg(m_mutexFeedData);
    {
        if(!m_connected){
            m_feedTransport.subscribe("coinbaseFeed", this);
            m_feedTransport.connect("wss://ws-feed.pro.coinbase.com");
            m_connected = true;
            
        }
        auto it = m_prodIdToListener.find(productId);
        if(it == m_prodIdToListener.end()){
            m_prodIdToListener.emplace(
                std::make_pair(productId, std::make_unique<CoinbaseFeedProduct>(productId, listener)));
        } else {
            std::cout << "listener already registered";
        }
        std::cout << "Registering listener for prod id: " << productId << std::endl;
    }
    m_feedTransport.send(generateSubscribeMessage(productId));
}

void CoinbaseFeedClient::unsubscribe(const std::string& productId) {
    std::cout << "Unsubscribed " << productId << std::endl;
    m_feedTransport.send(generateUnsubscribeMessage(productId));
    std::lock_guard<std::mutex> lg(m_mutexFeedData);
    m_prodIdToListener.erase(productId);
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