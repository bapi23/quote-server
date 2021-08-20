#include <memory>
#include <nlohmann/json.hpp>

#include "coinbase/CoinbaseFeedMessageHandler.hpp"


#include "product/ProductChangeListener.hpp"
#include "product/ProductChangeResetOrderBook.hpp"
#include "product/ProductChangeOpen.hpp"
#include "product/ProductChangeDone.hpp"
#include "product/ProductChangeMatch.hpp"
#include "product/ProductChangeChange.hpp"
#include "MessageReceiver.hpp"
#include "Order.hpp"


CoinbaseFeedMessageHandler::CoinbaseFeedMessageHandler(ProductChangeListener* listener, const std::string& productId):m_listener(listener), m_productId(productId){
}

void CoinbaseFeedMessageHandler::onMessageReceived(const nlohmann::json& jmsg) {
    //.. create change
    auto pc = getProductChange(jmsg);
    std::cout << "DUPA";
    if(pc)
        m_listener->onProductChange(std::move(pc)); //make shared from this -> MARKET!
}

std::unique_ptr<ProductChange> CoinbaseFeedMessageHandler::getProductChange(const nlohmann::json& jmsg){
    std::cout << "DUPA14.6" << std::endl;
    std::unique_ptr<ProductChange> pc;

    if(jmsg.contains("bids") && jmsg.contains("asks")){
        std::cout << "[FULL ORDER BOOK]" << std::endl;
        std::vector<Order> bids;
        std::vector<Order> asks;

        for(const auto& order: jmsg["asks"]){
            const std::string price = order[0].get<std::string>();
            const std::string size = order[1].get<std::string>();
            const std::string orderId = order[2].get<std::string>();
            asks.push_back(Order{std::stod(price), std::stod(size), orderId});
        }
        for(const auto& order: jmsg["bids"]){
            const std::string price = order[0].get<std::string>();
            const std::string size = order[1].get<std::string>();
            const std::string orderId = order[2].get<std::string>();
            bids.push_back(Order{std::stod(price), std::stod(size), orderId});
        }
        std::cout << "DUPA3" << std::endl;
        std::vector<std::unique_ptr<ProductChange>> toMerge;
        std::cout << "DUPA4.3" << std::endl;
        if(jmsg.contains("to_merge")){
            std::cout << "DUPA4.4" << std::endl;
            for(const auto& msg: jmsg["to_merge"]){
                std::cout << "DUPA4.5" << std::endl;
                std::cout << msg;
                std::cout << "DUPA4.55" << std::endl;
                auto pc = getProductChange(msg);
                std::cout << "DUPA4.6" << std::endl;
                if(pc){
                    std::cout << "DUPA4.7" << std::endl;
                    toMerge.push_back(std::move(pc));
                    std::cout << "DUPA4.8" << std::endl;
                }
            }
        }
        std::cout << "DUPA4" << std::endl;
        std::cout << "Product changes to merge " << toMerge.size() << std::endl;
        pc = std::make_unique<ProductChangeResetOrderBook>(bids, asks, std::move(toMerge), m_productId);
        std::cout << "DUPA5" << std::endl;
    } else if (jmsg.contains("type")) {
        if(jmsg["type"].get<std::string>() == "open"){
            const std::string orderId = jmsg["order_id"].get<std::string>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["remaining_size"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
            pc = std::make_unique<ProductChangeOpen>(orderId, std::stod(price), std::stod(size), side, m_productId);
        } else if (jmsg["type"].get<std::string>() == "done") {
            std::cout << "DUPA7.6" << std::endl;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << jmsg["order_id"].get<std::string>() << std::endl;
            std::cout << jmsg;
            const std::string orderId = jmsg["order_id"].get<std::string>();
            std::cout << "DUPA7.651" << std::endl;
            std::cout << jmsg;
            const std::string sideStr =  jmsg["side"].get<std::string>();
            std::cout << "DUPA7.65555555555555" << std::endl;
            const Side side = sideStr == "sell"? Side::Sell: Side::Buy;
            std::cout << "DUPA7.7" << std::endl;
            pc = std::make_unique<ProductChangeDone>(orderId, side, m_productId);
        } else if (jmsg["type"].get<std::string>() == "match") {
            const std::string makerOrderId = jmsg["maker_order_id"].get<std::string>();
            const std::string takerOrderId = jmsg["taker_order_id"].get<std::string>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["size"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " maker id: " << makerOrderId << " taker id: " << takerOrderId <<std::endl;
            pc = std::make_unique<ProductChangeMatch>(makerOrderId, takerOrderId, std::stod(price), std::stod(size), side, m_productId);

        } else if (jmsg["type"].get<std::string>() == "change") {
            const std::string orderId = jmsg["order_id"].get<std::string>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["size"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
            pc = std::make_unique<ProductChangeChange>(orderId, std::stod(price), std::stod(size), side, m_productId);
        } else if (jmsg["type"].get<std::string>() == "ticker") {
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: "  << std::endl;
            //TODO
        } else if (jmsg["type"].get<std::string>() == "received") {
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: "  << std::endl;
            //TODO
        } else if (jmsg["type"].get<std::string>() == "heartbeat") {
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << std::endl;
            //TODO
        } else {
            std::cout << "Unsupported message type: " << jmsg << std::endl;
        }
    } else {
        std::cout << "Can't handle message:" << jmsg << std::endl;
    }
    return pc;
}