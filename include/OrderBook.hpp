#pragma once

#include <vector>
#include <unordered_map>

#include "Order.hpp"

class OrderBook{
public:
    void init(std::vector<Order> bids, std::vector<Order> asks){
        m_bids.clear();
        m_asks.clear();
        for(const auto& bid: bids){
            m_bids.push_back(bid);
        }
        for(const auto& ask: asks){
            m_asks.push_back(ask);
        }
    }

    bool addOrder(const Order& order){

    }

    bool removeOrder(const Order& order){

    }

    bool updateOrder(const Order& order){

    }

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
    std::unordered_map<std::string, Order> m_idToBid;
    std::unordered_map<std::string, Order> m_idToAsk;
};