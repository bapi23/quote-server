#pragma once

#include "product/Product.hpp"
#include "Side.hpp"
#include "utils/FloatingP.hpp"

class ProductChangeDone: public ProductChange{
public:
    ProductChangeDone(const std::string& orderId,
                       Side side,
                      std::string productId,
                      unsigned long sequenceNumber):
                ProductChange(productId, sequenceNumber),
                m_orderId(orderId),
                m_side(side)

    {
    }

    bool updateOrderBook(OrderBook* orderBook) const override{
        orderBook->removeOrder(m_orderId, m_side);
        return true;
    }

private:
    std::string m_orderId;
    Side m_side;
};