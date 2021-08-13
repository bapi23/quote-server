#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"
#include "ProductChangePublisher.hpp"

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

    void publish(const std::string data){
        zmq::message_t message(data.size());
        memcpy (message.data(), data.data(), data.size());
        m_sock.send(message, zmq::send_flags::none);
    }

    zmq::context_t m_ctx;
    zmq::socket_t m_sock;
};