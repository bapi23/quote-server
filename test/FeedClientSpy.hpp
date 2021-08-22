#pragma once

#include "FeedClient.hpp"
#include <unordered_map>
#include "product/ProductChangeListener.hpp"

class FeedClientSpy: public FeedClient
{
public:
    virtual void subscribe(const std::string& product_id, ProductChangeListener* listener) override{
        m_listeners[product_id] = listener;
    }
    virtual void unsubscribe(const std::string& product_id) override {
        m_listeners.erase(product_id);
    }

std::unordered_map<std::string, ProductChangeListener*> m_listeners;
};