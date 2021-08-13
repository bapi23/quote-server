#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"
#include "OrderBookView.hpp"

class OrderBookPublisher
{
public:
    virtual void publish(const OrderBookView& view) = 0;
    virtual ~OrderBookPublisher(){};
};