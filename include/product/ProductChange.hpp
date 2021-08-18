#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "trade/Trade.hpp"

class OrderBook;

class ProductChange{
public:
    ProductChange(std::string productId): m_productId(productId){}
    virtual std::string getProductId(){return m_productId;}
    
    virtual std::vector<std::unique_ptr<Trade>> getTrades() const = 0;
    virtual bool updateOrderBook(OrderBook* orderBook) const = 0;
    virtual ~ProductChange(){};

private:
    std::string m_productId;
};