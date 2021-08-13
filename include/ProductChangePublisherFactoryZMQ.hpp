#pragma once
#include "ProductChangePublisherZMQ.hpp"

class ProductChangePublisherFactoryZMQ: public ProductChangePublisherFactory {
public:
    std::unique_ptr<ProductChangePublisher> createPublisher(const std::string& productId) override{
        return std::make_unique<ProductChangePublisherZMQ>(productId);
    }
};