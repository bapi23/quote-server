#pragma once

#include <memory>

#include "trade/Trade.hpp"

class OrderBook;

class TradeListener{
public:
    virtual void onTrade(std::unique_ptr<Trade> trade) = 0;
    virtual ~TradeListener(){};
};