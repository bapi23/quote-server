#pragma once
#include "product/Product.hpp"

class ProductChangeResetOrderBook: public ProductChange{
public:
    ProductChangeResetOrderBook(const std::vector<Order>& bids, const std::vector<Order>& asks, std::vector<std::unique_ptr<ProductChange>>&& toMerge, const std::string& productId):
        ProductChange(productId),
        m_bids(bids),
        m_asks(asks),
        m_toMerge(std::move(toMerge))
    {
    }

    std::vector<std::unique_ptr<Trade>> getTrades() const override{
        return std::vector<std::unique_ptr<Trade>>();
    }

    bool updateOrderBook(OrderBook* orderBook) const override{
        std::cout << "Reinitializing orderbook with " << m_bids.size() << " bids, "
                                                      << m_asks.size() << " asks and "
                                                      << m_toMerge.size() << " updates" << std::endl; 
        
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        orderBook->init(m_bids, m_asks);

        for(const auto& pc: m_toMerge){
            pc->updateOrderBook(orderBook);
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        std::cout << "Reinitializing took = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
        return true;
    }

private:
    std::vector<Order> m_bids;
    std::vector<Order> m_asks;
    std::vector<std::unique_ptr<ProductChange>> m_toMerge;
};