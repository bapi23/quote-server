#pragma once
#include <iostream>

#include <zmq_addon.hpp>
#include "product/ProductIdConnectionTable.hpp"

class ZMQPublisher{
public:
    ZMQPublisher(const std::string& productId){
    }

    void publish(const std::string& payload){
    }

private:
    zmq::context_t m_ctx;
    zmq::socket_t m_sock;
};