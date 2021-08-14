#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"
#include "ProductChangePublisher.hpp"

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
        m_sock(m_ctx, zmq::socket_type::pub)
        {
            if(productId.find(productId))
                throw std::runtime_error("Can't find product id in connection map!");
            m_sock.bind("tcp://*:" + transport::prodIdToPort[productId]);
        }

    void publish(OrderBookView* view){
        nlohmann::json jmessage;
        nlohmann::json test = view->getAsks()[0];
        
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
    }

    void publish(std::unique_ptr<Trade> trade){
        std::string payload = trade->generateMessage();
        zmq::message_t message(payload.size());
        memcpy (message.data(), payload.data(), payload.size());
        m_sock.send(message, zmq::send_flags::none);
    }

    zmq::context_t m_ctx;
    zmq::socket_t m_sock;
};