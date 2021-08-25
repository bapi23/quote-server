#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <memory>

#include "ProductIdConnectionTable.hpp"
#include "OrderBookView.hpp"

class ProductChangePublisher
{
public:
    virtual void publish(OrderBookView* view) = 0;
    virtual ~ProductChangePublisher(){};
};