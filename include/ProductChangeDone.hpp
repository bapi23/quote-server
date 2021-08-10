#include "Product.hpp"
#include "Side.hpp"
#include "TradeDone.hpp"
#include "utils/FloatingP.hpp"

class ProductChangeDone: public ProductChange{
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


    ProductChangeDone(const std::string& orderId,
                       Side side):
                m_orderId(orderId),
                m_side(side)

    {
    }

    std::vector<std::unique_ptr<Trade>> getTrades() const override{
        std::vector<std::unique_ptr<Trade>> vec;
        vec.push_back(std::make_unique<TradeDone>(m_orderId));
        return vec;
    }

    bool updateOrderBook(OrderBook& orderBook) const override{
        orderBook.removeOrder(m_orderId, m_side);
        return true;
    }

private:
    std::string m_orderId;
    Side m_side;
};