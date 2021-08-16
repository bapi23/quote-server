#include "product/Product.hpp"
#include "Side.hpp"
#include "trade/TradeMatch.hpp"
#include "utils/FloatingP.hpp"

class ProductChangeMatch: public ProductChange{
public:

    // "type": "match",
    // "trade_id": 10,
    // "sequence": 50,
    // "maker_order_id": "ac928c66-ca53-498f-9c13-a110027a60e8",
    // "taker_order_id": "132fb6ae-456b-4654-b4e0-d681ac05cea1",
    // "time": "2014-11-07T08:19:27.028459Z",
    // "product_id": "BTC-USD",
    // "size": "5.23512",
    // "price": "400.23",
    // "side": "sell"


    ProductChangeMatch(const std::string& makerOrderId,
                       const std::string& takerOrderId, 
                       FloatingP price, 
                       FloatingP size, 
                       Side side):
                m_makerOrderId(makerOrderId),
                m_takerOrderId(takerOrderId),
                m_price(price),
                m_size(size),
                m_side(side)

    {
    }

    std::vector<std::unique_ptr<Trade>> getTrades() const override{
        std::vector<std::unique_ptr<Trade>> vec;
        vec.push_back(std::make_unique<TradeMatch>(m_makerOrderId, m_size));
        vec.push_back(std::make_unique<TradeMatch>(m_takerOrderId, m_size));
        return vec;
    }

    bool updateOrderBook(OrderBook* orderBook) const override{
        orderBook->matchOrders(m_makerOrderId, m_takerOrderId, m_size, m_side);
        return true;
    }

private:
    std::string m_makerOrderId;
    std::string m_takerOrderId;
    FloatingP m_price;
    FloatingP m_size;
    Side m_side;
};