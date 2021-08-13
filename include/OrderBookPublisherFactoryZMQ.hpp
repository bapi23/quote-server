#pragma once
#include "OrderBookPublisherZMQ.hpp"

class OrderBookPublisherFactoryZMQ: public OrderBookPublisherFactory {
public:
    std::unique_ptr<OrderBookPublisher> createPublisher(const std::string& productId) override{
        return std::make_unique<OrderBookPublisherZMQ>(productId);
    }
};