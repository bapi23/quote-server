#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"

class ProductChangePublisher
{
public:
    virtual void publish(const std::string data) = 0;
    virtual ~ProductChangePublisher(){};
};