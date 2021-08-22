
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "OrderBook.hpp"
#include "Order.hpp"
#include "utils/FloatingP.hpp"
#include "Side.hpp"
#include "OrderBookView.hpp"


void OrderBook::init(std::vector<Order> bids, std::vector<Order> asks){
    m_bids.clear();
    m_asks.clear();
    for(const auto& bid: bids){
        m_bids.push_back(bid);
    }
    for(const auto& ask: asks){
        m_asks.push_back(ask);
    }
}

void OrderBook::addOrder(const Order& order, Side side){
    if(side == Side::Buy){
        m_bids.push_back(order);
    } else {
        m_asks.push_back(order);
    }
}

void OrderBook::removeOrder(const std::string& orderId, Side side){
    if(side == Side::Buy){
        auto it = std::find_if(m_asks.begin(), m_asks.end(), 
                            [&orderId](const Order& o){ return o.order_id == orderId; });
        if(it != m_asks.end())
            m_asks.erase(it);
    } else {
        auto it = std::find_if(m_bids.begin(), m_bids.end(), 
                            [&orderId](const Order& o){ return o.order_id == orderId; });
        if(it != m_bids.end())
            m_bids.erase(it);
    }
}

void OrderBook::updateOrder(const std::string orderId, FloatingP newSize, Side side){
    if(side == Side::Buy){
        auto it = std::find_if(m_asks.begin(), m_asks.end(), 
                            [&orderId](const Order& o){ return o.order_id == orderId; });
        if(it == m_asks.end()){
            std::cout << "Can't update - order doesn't exists! Order id: " << orderId << std::endl;
            return;
        }
        it->size = newSize;
    } else {
        auto it = std::find_if(m_bids.begin(), m_bids.end(), 
                            [&orderId](const Order& o){ return o.order_id == orderId; });
        if(it == m_bids.end()){
            std::cout << "Can't update - order doesn't exists! Order id: " << orderId << std::endl;
            return;
        }
        it->size = newSize;
    }
}

void OrderBook::matchOrders(const std::string& makerOrderId,
                const std::string& takerOrderId, 
                FloatingP matchedSize,
                Side side){
    if(side == Side::Sell){
        auto it = std::find_if(m_bids.begin(), m_bids.end(), 
                [&makerOrderId](const Order& o){ return o.order_id == makerOrderId; });
        if(it == m_bids.end()){
            std::cout << "Can't update - matched order doesn't exists! Order id: " << makerOrderId << std::endl;
            return;
        }
        it->size = it->size - matchedSize;

        auto ita = std::find_if(m_asks.begin(), m_asks.end(), 
                [&takerOrderId](const Order& o){ return o.order_id == takerOrderId; });
        if(ita == m_asks.end()){
            std::cout << "Can't update - matched order doesn't exists! Order id: " << takerOrderId << std::endl;
            return;
        }
        ita->size = ita->size - matchedSize;
    } else {
        auto it = std::find_if(m_asks.begin(), m_asks.end(), 
                [&makerOrderId](const Order& o){ return o.order_id == makerOrderId; });
        if(it == m_asks.end()){
            std::cout << "Can't update - matched order doesn't exists! Order id: " << makerOrderId << std::endl;
            return;
        }
        it->size = it->size - matchedSize;

        auto itb = std::find_if(m_bids.begin(), m_bids.end(), 
                [&takerOrderId](const Order& o){ return o.order_id == takerOrderId; });
        if(itb == m_bids.end()){
            std::cout << "Can't update - meched order doesn't exists! Order id: " << takerOrderId << std::endl;
            return;
        }
        itb->size = itb->size - matchedSize;
    }
}

//OrderBookView interface
const std::vector<Order>& OrderBook::getAsks() const {
    return m_asks;
}

const std::vector<Order>& OrderBook::getBids() const {
    return m_bids;
}