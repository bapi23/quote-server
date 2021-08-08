#pragma once

struct Order{
    int price; 
    int size;
    std::string order_id;
};

class OrderBook{
public:
    bool addOrder(const Order& order){

    }

    bool removeOrder(const Order& order){

    }

    bool updateOrder(const Order& order){

    }

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
    std::unordered_map<std::string, Order> m_bids;
    std::unordered_map<std::string, Order> m_asks;
};