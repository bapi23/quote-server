#include "Product.hpp"

class ResetProductOrderbookChange: public ProductChange{
public:
    ResetProductOrderbookChange(const std::vector<Order>& bids, const std::vector<Order>& asks):
        m_bids(bids),
        m_asks(asks)
    {
    }

    std::vector<Trade> getTrades() const override{
        return std::vector<Trade>();
    }

    bool updateOrderBook(OrderBook& orderBook) const {
        orderBook.init(m_bids, m_asks);
        return true;
    }

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
};