#pragma once

#include <string>

#include <ProductChangeListener.hpp>

class FeedClient{
public:
    virtual void subscribe(const std::string& product_id, ProductChangeListener* listener);
    virtual void unsubscribe(const std::string& product_id);
    virtual ~FeedClient(){};
};