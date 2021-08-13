#pragma once

#include "OrderBookPublisher.hpp"

class OrderBookPublisherSpy: public OrderBookPublisher
{
public:
    void publish(const std::string data) override {
        messages.push_back(data);
    }

    std::vector<std::string> messages;
};