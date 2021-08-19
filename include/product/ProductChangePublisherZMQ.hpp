#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"
#include "product/ProductChangePublisher.hpp"

using json = nlohmann::json;

void to_json(nlohmann::json& j, const Order& o) {
    j = json{{"order_id", o.order_id}, {"size", o.size}, {"price", o.price}};
}

void from_json(const json& j, Order& o) {
    j.at("order_id").get_to(o.order_id);
    j.at("size").get_to(o.size);
    j.at("price").get_to(o.price);
}

class ProductChangePublisherZMQ: public ProductChangePublisher
{
public:
    ProductChangePublisherZMQ(const std::string& productId):
        m_ctx(),
        m_sock(m_ctx, zmq::socket_type::pub),
        m_productId(productId)
        {
            if(productId.find(productId))
                throw std::runtime_error("Can't find product id in connection map!");
            std::cout << "Binding product to uri: tcp://127.0.0.1:" + transport::prodIdToPort[productId];
            m_sock.bind("tcp://127.0.0.1:" + transport::prodIdToPort[productId]);
        }

    void publish(OrderBookView* view){
        std::chrono::steady_clock::time_point newTime = std::chrono::steady_clock::now();
        auto difference = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
        lastTime = newTime;

        //std::cout << "frequency = " << 1.0f/(float(difference)/1000000.0f) << "[Hz]" << std::endl;
        nlohmann::json jmessage = {
            {"asks", {}},
            {"bids", {}},
            {"message", "orderbook"},
            {"product_id", m_productId}
        };
        
        for(const auto& order: view->getAsks()){
            jmessage["asks"].push_back(order);
        }
        
        for(const auto& order: view->getBids()){
            jmessage["bids"].push_back(order);
        }
        std::string payload = jmessage.dump();
        zmq::message_t message(payload.size());
        memcpy (message.data(), payload.data(), payload.size());
        //std::cout << "Publishing order book for: [" << m_productId << "]" << std::endl;
        m_sock.send(message, zmq::send_flags::none);
    }

    void publish(std::unique_ptr<Trade> trade){
        std::string payload = trade->generateMessage();

        // TODO begin return nlohmann json or create TradeView!
        auto jmessage = nlohmann::json::parse(payload); 
        jmessage["product_id"] = m_productId;
        payload = jmessage.dump();
        // TODO end

        zmq::message_t message(payload.size());
        memcpy (message.data(), payload.data(), payload.size());
        //std::cout << "Publishing trade for: [" << m_productId << "]" << std::endl;
        m_sock.send(message, zmq::send_flags::none);
    }

    zmq::context_t m_ctx;
    zmq::socket_t m_sock;
    std::chrono::steady_clock::time_point lastTime;
    std::string m_productId;
};