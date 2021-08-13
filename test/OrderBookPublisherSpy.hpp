#pragma once

#include "OrderBookPublisher.hpp"

class OrderBookPublisherSpy: public OrderBookPublisher
{
public:
    void publish(OrderBookView* view) override {
        messages.push_back(data);
    }

    std::vector<std::string> messages;
};