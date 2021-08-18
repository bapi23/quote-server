#pragma once
#include "product/Product.hpp"

class ProductChangeResetOrderBook: public ProductChange{
public:
    ProductChangeResetOrderBook(const std::vector<Order>& bids, const std::vector<Order>& asks, const std::vector<std::unique_ptr<ProductChange>>& toMerge, const std::string& productId):
        ProductChange(productId),
        m_bids(bids),
        m_asks(asks)
    {
    }

    std::vector<std::unique_ptr<Trade>> getTrades() const override{
        return std::vector<std::unique_ptr<Trade>>();
    }

    bool updateOrderBook(OrderBook* orderBook) const override{
        orderBook->init(m_bids, m_asks);
        // ret
        //for(const auto& pc: toMerge){
        //    ret +=  pc.updateOrderBook(orderBook);
        //}
        //return ret;
        return true;
    }

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
};