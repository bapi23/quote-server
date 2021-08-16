#pragma once

#include <memory>

#include "product/ProductChangeListener.hpp"
#include "product/ProductChangeResetOrderBook.hpp"
#include "product/ProductChangeOpen.hpp"
#include "product/ProductChangeDone.hpp"
#include "product/ProductChangeMatch.hpp"
#include "product/ProductChangeChange.hpp"
#include "MessageReceiver.hpp"
#include "Order.hpp"

using json = nlohmann::json;

class CoinbaseFeedMessageHandler: public MessageReceiver{
public:
    CoinbaseFeedMessageHandler(std::weak_ptr<ProductChangeListener> listener):m_listener(listener){

    }
    void onMessageReceived(const std::string& msg) override{
        //.. create change
        auto jmsg = json::parse(msg);
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
            pc = std::make_unique<ProductChangeResetOrderBook>(bids, asks);
        } else if (jmsg.contains("type")) {
            if(jmsg["type"].get<std::string>() == "open"){
                const std::string orderId = jmsg["order_id"].get<std::string>();
                const std::string price = jmsg["price"].get<std::string>();
                const std::string size = jmsg["remaining_size"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
                pc = std::make_unique<ProductChangeOpen>(orderId, std::stod(price), std::stod(size), side);
            } else if (jmsg["type"].get<std::string>() == "done") {
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << jmsg["order_id"].get<std::string>() << std::endl;
                const std::string orderId = jmsg["order_id"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
                pc = std::make_unique<ProductChangeDone>(orderId, side);
            } else if (jmsg["type"].get<std::string>() == "match") {
                const std::string makerOrderId = jmsg["maker_order_id"].get<std::string>();
                const std::string takerOrderId = jmsg["taker_order_id"].get<std::string>();
                const std::string price = jmsg["price"].get<std::string>();
                const std::string size = jmsg["size"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " maker id: " << makerOrderId << " taker id: " << takerOrderId <<std::endl;
                pc = std::make_unique<ProductChangeMatch>(makerOrderId, takerOrderId, std::stod(price), std::stod(size), side);

            } else if (jmsg["type"].get<std::string>() == "change") {
                const std::string orderId = jmsg["order_id"].get<std::string>();
                const std::string price = jmsg["price"].get<std::string>();
                const std::string size = jmsg["size"].get<std::string>();
                const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
                std::cout << "Received feed message:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
                pc = std::make_unique<ProductChangeChange>(orderId, std::stod(price), std::stod(size), side);
            } else {
                //std::cout << "Unsupported message type: " << jmsg["type"].get<std::string>() << std::endl;
            }
        } else {
            std::cout << "Can't handle message" << msg;
        }
        if(pc != nullptr){
            auto shared = m_listener.lock();
            assert(shared);
            if(shared)
                shared->onProductChange(std::move(pc));
            else
                std::cout << "Listener doesn't exists!";
        }
    }

private:
    std::weak_ptr<ProductChangeListener> m_listener;
};