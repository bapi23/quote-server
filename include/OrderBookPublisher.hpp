#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ProductIdConnectionTable.hpp"
#include "OrderBookView.hpp"
#include "Trade.hpp"

class OrderBookPublisher
{
public:
    virtual void publish(OrderBookView* view) = 0;
    virtual void publish(std::unique_ptr<Trade> trade) = 0;
    virtual ~OrderBookPublisher(){};
};