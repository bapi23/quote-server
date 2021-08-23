#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "trade/Trade.hpp"

class OrderBook;

class ProductChange{
public:
    ProductChange(const std::string& productId, long unsigned sequenceNumber): 
        m_productId(productId),
        m_sequenceNumber(sequenceNumber){}
    virtual std::string getProductId(){return m_productId;}
    long unsigned getSequenceNumber(){return m_sequenceNumber;}
    
    virtual std::vector<std::unique_ptr<Trade>> getTrades() const = 0;
    virtual bool updateOrderBook(OrderBook* orderBook) const = 0;
    virtual ~ProductChange(){};

private:
    std::string m_productId;
    unsigned long m_sequenceNumber;
};