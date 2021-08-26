#pragma once
#include <iostream>

#include <zmq_addon.hpp>
#include "CoinbaseEndpointService.hpp"

class ZMQPublisher{
public:
    ZMQPublisher(const std::string& address):
        m_ctx(),
        m_sock(m_ctx, zmq::socket_type::pub){
        std::cout << "Binding product to uri: " << address << std::endl;
        bind_addr = "tcp://" + address;
        m_sock.bind(bind_addr);
    }

    void publish(const std::string& payload){
        zmq::message_t message(payload.size());
        memcpy (message.data(), payload.data(), payload.size());
        m_sock.send(message, zmq::send_flags::none);
    }
    
    ~ZMQPublisher(){
        m_sock.unbind(bind_addr);
    }

    ZMQPublisher(const ZMQPublisher&) = delete;
    ZMQPublisher& operator=(const ZMQPublisher&) = delete;

private:
    std::string bind_addr;
    zmq::context_t m_ctx;
    zmq::socket_t m_sock;
};