#pragma once
#include "ProductChangePublisher.hpp"

class ProductChangePublisherFactory{
public:
    virtual std::unique_ptr<ProductChangePublisher> createPublisher(const std::string& productId) = 0;
    virtual ~ProductChangePublisherFactory(){};
};