#pragma once

#include "product/Product.hpp"
#include "Side.hpp"

#include "trade/TradeActivate.hpp"

class ProductChangeActivate: public ProductChange{
public:
    // "type": "open",
    // "time": "2014-11-07T08:19:27.028459Z",
    // "product_id": "BTC-USD",
    // "sequence": 10,
    // "order_id": "d50ec984-77a8-460a-b958-66f114b0de9b",
    // "price": "200.2",
    // "remaining_size": "1.00",
    // "side": "sell"

    ProductChangeActivate(const std::string& orderId, 
                      FloatingP price, 
                      FloatingP size, 
                      Side side,
                      FloatingP founds,
                      FloatingP stopPrice,
                      const std::string& stopType,
                      std::string productId,
                      unsigned long sequenceNumber):
                ProductChange(productId, sequenceNumber),
                m_orderId(orderId),
                m_price(price),
                m_size(size),
                m_side(side),
                m_founds(founds),
                m_stopPrice(stopPrice),
                m_stopType(stopType)

    {
    }

    std::vector<std::unique_ptr<Trade>> getTrades() const override{
        std::vector<std::unique_ptr<Trade>> out;

        out.push_back(std::make_unique<TradeActivate>(m_orderId, m_size, m_price, m_founds, m_stopPrice, m_stopType, getProductId()));
        return out;
    }

    bool updateOrderBook(OrderBook* orderBook) const override{
        return false;
    }

private:
    std::string m_orderId;
    FloatingP m_price;
    FloatingP m_size;
    Side m_side;
    FloatingP m_founds;
    FloatingP m_stopPrice;
    std::string m_stopType;
};