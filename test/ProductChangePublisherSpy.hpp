#pragma once

#include "product/ProductChangePublisher.hpp"

class ProductChangePublisherSpy: public ProductChangePublisher
{
public:
    void publish(OrderBookView* view) override {
        m_asks = view->getAsks();
        m_bids = view->getBids();
    }
    
    void publish(std::unique_ptr<Trade> trade) override {
        m_trades.push_back(std::move(trade));
    }

    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
    std::vector<std::unique_ptr<Trade>> m_trades;
};