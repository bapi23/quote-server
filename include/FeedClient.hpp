#pragma once

#include <string>

#include "product/ProductChangeListener.hpp"

class FeedClient{
public:
    virtual void subscribe(const std::string& product_id, std::weak_ptr<ProductChangeListener> listener) = 0;
    virtual void unsubscribe(const std::string& product_id) = 0;
    virtual ~FeedClient(){};
};