#pragma once

#include <memory>

#include "ProductChangeListener.hpp"
#include "ResetProductOrderbookChange.hpp"
#include "ProductChange.hpp"
#include "MessageReceiver.hpp"
#include "Order.hpp"

using json = nlohmann::json;

class CoinbaseFeedMessageHandler: public MessageReceiver{
public:
    CoinbaseFeedMessageHandler(ProductChangeListener* listener):m_listener(listener){

    }
    void onMessageReceived(const std::string& msg) override{
        //.. create change
        auto jmsg = json::parse(msg);
        std::unique_ptr<ProductChange> pc;
    
        if(jmsg.contains("bids") && jmsg.contains("asks")){
            std::vector<Order> bids;
            std::vector<Order> asks;
            for(const auto& order: jmsg["asks"]){
                const double price = order[0].get<double>();
                const double size = order[0].get<double>();
                const std::string orderId = order[0].get<std::string>();

                asks.push_back(Order{price, size, orderId});
            }
            for(const auto& order: jmsg["bids"]){
                const double price = order[0].get<double>();
                const double size = order[0].get<double>();
                const std::string orderId = order[0].get<std::string>();

                bids.push_back(Order{price, size, orderId});
            }
            pc = std::make_unique<ResetProductOrderbookChange>(bids, asks);
        } else {
            std::cout << "Can't handle message" << msg;
        }
        m_listener->onProductChange(std::move(pc));
    }

private:
    ProductChangeListener* m_listener;
};