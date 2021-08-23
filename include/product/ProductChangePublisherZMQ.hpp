#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>
#include <boost/numeric/conversion/cast.hpp>

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
        m_productId(productId),
        m_stamps(2000)
        {
            if(productId.find(productId))
                throw std::runtime_error("Can't find product id in connection map!");
            std::cout << "Binding product to uri: tcp://127.0.0.1:" + transport::prodIdToPort[productId];
            m_sock.bind("tcp://127.0.0.1:" + transport::prodIdToPort[productId]);
        }

    void publish(OrderBookView* view){
        // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        // m_stamps.push_back(begin);
        // if(m_stamps.size() > 0){
        //     auto last = m_stamps.back();
        //     auto first = m_stamps.front();
        //     auto difference_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last - first).count();
        //     auto messages_per_second = boost::numeric_cast<float>(difference_ms)/boost::numeric_cast<float>(m_stamps.size()) * 1000;
        //     std::cout << "[" << m_productId << "] " << "Feed frequency = " << messages_per_second << " messages/s" << std::endl;
        //     // Send metrics
        // }

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
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
        m_sock.send(message, zmq::send_flags::none);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto difference = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        std::cout << "Publishing took" << difference << " micro seconds" << std::endl;
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
    std::string m_productId;
    boost::circular_buffer<std::chrono::steady_clock::time_point> m_stamps;
};