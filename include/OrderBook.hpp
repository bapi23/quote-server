#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "Order.hpp"
#include "utils/FloatingP.hpp"
#include "Side.hpp"
#include "OrderBookView.hpp"

class OrderBook: public OrderBookView{
public:
    void init(std::vector<Order> bids, std::vector<Order> asks);

    void addOrder(const Order& order, Side side);

    void removeOrder(const std::string& orderId, Side side);

    void updateOrder(const std::string orderId, FloatingP newSize, Side side);

    void matchOrders(const std::string& makerOrderId,
                    const std::string& takerOrderId, 
                    FloatingP matchedSize,
                    Side side);

    //OrderBookView interface
    const std::vector<Order>& getAsks() const override;
    const std::vector<Order>& getBids() const override;

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
};