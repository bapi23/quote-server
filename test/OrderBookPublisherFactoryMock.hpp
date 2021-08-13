#pragma once

#include "OrderBookPublisherFactory.hpp"
#include "OrderBookPublisherSpy.hpp"

class OrderBookPublisherFactoryMock: public OrderBookPublisherFactory
{
public:
    std::unique_ptr<OrderBookPublisher> createPublisher(const std::string& productId) override{
        auto pub = std::make_unique<OrderBookPublisherSpy>();
        pubPtr = pub.get();
        return std::move(pub);
    }

    OrderBookPublisherSpy* pubPtr;
};