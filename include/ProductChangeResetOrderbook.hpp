#include "Product.hpp"

class ProductChangeResetOrderbook: public ProductChange{
public:
    ProductChangeResetOrderbook(const std::vector<Order>& bids, const std::vector<Order>& asks):
        m_bids(bids),
        m_asks(asks)
    {
    }

    std::vector<std::unique_ptr<Trade>> getTrades() const override{
        return std::vector<std::unique_ptr<Trade>>();
    }

    bool updateOrderBook(OrderBook& orderBook) const override{
        orderBook.init(m_bids, m_asks);
        return true;
    }

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
};