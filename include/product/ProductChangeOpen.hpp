#pragma once

#include "product/Product.hpp"
#include "Side.hpp"

class ProductChangeOpen: public ProductChange{
public:
    // "type": "open",
    // "time": "2014-11-07T08:19:27.028459Z",
    // "product_id": "BTC-USD",
    // "sequence": 10,
    // "order_id": "d50ec984-77a8-460a-b958-66f114b0de9b",
    // "price": "200.2",
    // "remaining_size": "1.00",
    // "side": "sell"

    ProductChangeOpen(const std::string& orderId, 
                      FloatingP price, 
                      FloatingP remainingSize, 
                      Side side,
                      std::string productId,
                      unsigned long sequenceNumber):
                ProductChange(productId, sequenceNumber),
                m_orderId(orderId),
                m_price(price),
                m_remainingSize(remainingSize),
                m_side(side)

    {
    }

    bool updateOrderBook(OrderBook* orderBook) const override{
        orderBook->addOrder(Order{m_price, m_remainingSize, m_orderId}, m_side);
        return true;
    }

private:
    std::string m_orderId;
    FloatingP m_price;
    FloatingP m_remainingSize;
    Side m_side;
};