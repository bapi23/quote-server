#pragma once
#include "OrderBookPublisher.hpp"

class OrderBookPublisherFactory{
public:
    virtual std::unique_ptr<OrderBookPublisher> createPublisher(const std::string& productId) = 0;
    virtual ~OrderBookPublisherFactory(){};
};