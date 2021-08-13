#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "Trade.hpp"

class OrderBook;

class ProductChange{
public:
    virtual std::vector<std::unique_ptr<Trade>> getTrades() const = 0;
    virtual bool updateOrderBook(OrderBook* orderBook) const = 0;
    virtual ~ProductChange(){};
};