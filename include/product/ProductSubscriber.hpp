#pragma once
#include <string>

class ProductSubscriber{
public:
    virtual void subscribe(const std::string& clientId, const std::string& prodId) = 0;
    virtual void unsubscribe(const std::string& clientId, const std::string& prodId) = 0;
    virtual ~ProductSubscriber(){};
};