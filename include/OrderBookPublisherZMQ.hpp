#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"
#include "OrderBookPublisher.hpp"

using json = nlohmann::json;

class OrderBookPublisherZMQ: public OrderBookPublisher
{
public:
    OrderBookPublisherZMQ(const std::string& productId):
        m_ctx(),
        m_sock(m_ctx, zmq::socket_type::pub)
        {
            if(productId.find(productId))
                throw std::runtime_error("Can't find product id in connection map!");
            m_sock.bind("tcp://*:" + transport::prodIdToPort[productId]);
        }

    void publish(OrderBookView* view){
        nlohmann::json jmessage;
        jmessage["asks"] = nlohmann::json::parse(view->getAsks());
        jmessage["bids"] = nlohmann::json::parse(view->getAsks());
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