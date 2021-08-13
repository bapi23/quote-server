#pragma once

#include <vector>
#include "Order.hpp"

class OrderBookView {
public:
    virtual const std::vector<Order>& getAsks() const = 0;
    virtual const std::vector<Order>& getBids() const = 0;
    virtual ~OrderBookView(){}
};