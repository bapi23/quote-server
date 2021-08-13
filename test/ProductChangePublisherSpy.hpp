#pragma once

#include "ProductChangePublisher.hpp"

class ProductChangePublisherSpy: public ProductChangePublisher
{
public:
    void publish(const std::string data) override {
        messages.push_back(data);
    }

    std::vector<std::string> messages;
};