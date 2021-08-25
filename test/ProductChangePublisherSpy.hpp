#pragma once

#include "product/ProductChangePublisher.hpp"

class ProductChangePublisherSpy: public ProductChangePublisher
{
public:
    void publish(OrderBookView* view) override {
        m_asks = view->getAsks();
        m_bids = view->getBids();
    }

    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
};