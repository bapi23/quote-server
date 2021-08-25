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
    if(pc)
        m_listener->onProductChange(std::move(pc)); //make shared from this -> MARKET!
}

std::unique_ptr<ProductChange> CoinbaseFeedMessageHandler::getProductChange(const nlohmann::json& jmsg) {
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
        std::vector<std::unique_ptr<ProductChange>> toMerge;
        if(jmsg.contains("to_merge")){
            for(const auto& msg: jmsg["to_merge"]){
                auto pc = getProductChange(msg);
                if(pc){
                    toMerge.push_back(std::move(pc));
                }
            }
        }
        std::cout << "Product changes to merge " << toMerge.size() << std::endl;
        long unsigned sequence = jmsg["sequence"].get<long unsigned>();
        pc = std::make_unique<ProductChangeResetOrderBook>(bids, asks, std::move(toMerge), m_productId, sequence);
    } else if (jmsg.contains("type")) {
        if(jmsg["type"].get<std::string>() == "open"){
            const std::string orderId = jmsg["order_id"].get<std::string>();
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["remaining_size"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
            pc = std::make_unique<ProductChangeOpen>(orderId, std::stod(price), std::stod(size), side, m_productId, sequence);
        } else if (jmsg["type"].get<std::string>() == "done") {
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << jmsg["order_id"].get<std::string>() << std::endl;
            const std::string orderId = jmsg["order_id"].get<std::string>();
            const std::string sideStr =  jmsg["side"].get<std::string>();
            const Side side = sideStr == "sell"? Side::Sell: Side::Buy;
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            pc = std::make_unique<ProductChangeDone>(orderId, side, m_productId, sequence);
        } else if (jmsg["type"].get<std::string>() == "match") {
            const std::string makerOrderId = jmsg["maker_order_id"].get<std::string>();
            const std::string takerOrderId = jmsg["taker_order_id"].get<std::string>();
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["size"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " maker id: " << makerOrderId << " taker id: " << takerOrderId <<std::endl;
            pc = std::make_unique<ProductChangeMatch>(makerOrderId, takerOrderId, std::stod(price), std::stod(size), side, m_productId, sequence);

        } else if (jmsg["type"].get<std::string>() == "change") {
            const std::string orderId = jmsg["order_id"].get<std::string>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["size"].get<std::string>();
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
            pc = std::make_unique<ProductChangeChange>(orderId, std::stod(price), std::stod(size), side, m_productId, sequence);
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
