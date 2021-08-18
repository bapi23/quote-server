#pragma once

#include <memory>
#include <nlohmann/json.hpp>


#include "product/ProductChangeListener.hpp"
#include "product/ProductChangeResetOrderBook.hpp"
#include "product/ProductChangeOpen.hpp"
#include "product/ProductChangeDone.hpp"
#include "product/ProductChangeMatch.hpp"
#include "product/ProductChangeChange.hpp"
#include "MessageReceiver.hpp"
#include "Order.hpp"

class CoinbaseFeedMessageHandler{
public:
    CoinbaseFeedMessageHandler(ProductChangeListener* listener, const std::string& productId):m_listener(listener), m_productId(productId){
    }

    void onMessageReceived(const nlohmann::json& jmsg) {
        //.. create change
        auto pc = getProductChange(jmsg);
        if(pc)
            m_listener->onProductChange(std::move(pc)); //make shared from this -> MARKET!
    }

private:
    std::unique_ptr<ProductChange> getProductChange(const nlohmann::json& jmsg){
        std::unique_ptr<ProductChange> pc;
    
        if(jmsg.contains("bids") && jmsg.contains("asks")){
            std::cout << "Received full orderbook message" << std::endl;
            std::vector<Order> bids;
            std::vector<Order> asks;

            for(const auto& order: jmsg["asks"]){
                const std::string price = order[0].get<std::string>();
                const std::string size = order[0].get<std::string>();
                const std::string orderId = order[0].get<std::string>();
                asks.push_back(Order{std::stod(price), std::stod(size), orderId});
            }
            for(const auto& order: jmsg["bids"]){
                const std::string price = order[0].get<std::string>();
                const std::string size = order[0].get<std::string>();
                const std::string orderId = order[0].get<std::string>();
                bids.push_back(Order{std::stod(price), std::stod(size), orderId});
            }
            std::vector<std::unique_ptr<ProductChange>> toMerge;
            if(jmsg.contains("to_merge")){
                for(const auto& msg: jmsg["to_merge"]){
                    auto pc = getProductChange(msg);
                    if(pc)
                        toMerge.push_back(std::move(pc));
                }
            }
            std::cout << "Product changes to merge" << toMerge.size() << std::endl;
            pc = std::make_unique<ProductChangeResetOrderBook>(bids, asks, std::move(toMerge), m_productId);
        } else if (jmsg.contains("type")) {
            if(jmsg["type"].get<std::string>() == "open"){
                const std::string orderId = jmsg["order_id"].get<std::string>();
                const std::string price = jmsg["price"].get<std::string>();
                const std::string size = jmsg["remaining_size"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
                pc = std::make_unique<ProductChangeOpen>(orderId, std::stod(price), std::stod(size), side, m_productId);
            } else if (jmsg["type"].get<std::string>() == "done") {
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << jmsg["order_id"].get<std::string>() << std::endl;
                const std::string orderId = jmsg["order_id"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                pc = std::make_unique<ProductChangeDone>(orderId, side, m_productId);
            } else if (jmsg["type"].get<std::string>() == "match") {
                const std::string makerOrderId = jmsg["maker_order_id"].get<std::string>();
                const std::string takerOrderId = jmsg["taker_order_id"].get<std::string>();
                const std::string price = jmsg["price"].get<std::string>();
                const std::string size = jmsg["size"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " maker id: " << makerOrderId << " taker id: " << takerOrderId <<std::endl;
                pc = std::make_unique<ProductChangeMatch>(makerOrderId, takerOrderId, std::stod(price), std::stod(size), side, m_productId);

            } else if (jmsg["type"].get<std::string>() == "change") {
                const std::string orderId = jmsg["order_id"].get<std::string>();
                const std::string price = jmsg["price"].get<std::string>();
                const std::string size = jmsg["size"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
                pc = std::make_unique<ProductChangeChange>(orderId, std::stod(price), std::stod(size), side, m_productId);
            } else if (jmsg["type"].get<std::string>() == "ticker") {
                //TODO
            } else if (jmsg["type"].get<std::string>() == "received") {
            
            } else {
               //std::cout << "Unsupported message type: " << jmsg << std::endl;
            }
        } else {
            //std::cout << "Can't handle message:"<< jmsg << std::endl;
        }
        return pc;
    }

    ProductChangeListener* m_listener;
    std::string m_productId;
};