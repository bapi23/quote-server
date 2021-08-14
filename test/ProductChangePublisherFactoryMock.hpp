#pragma once

#include "ProductChangePublisherFactory.hpp"
#include "ProductChangePublisherSpy.hpp"

class ProductChangePublisherFactoryMock: public ProductChangePublisherFactory
{
public:
    std::unique_ptr<ProductChangePublisher> createPublisher(const std::string& productId) override{
        auto pub = std::make_unique<ProductChangePublisherSpy>();
        pubPtr = pub.get();
        return std::move(pub);
    }

    ProductChangePublisherSpy* pubPtr;
};