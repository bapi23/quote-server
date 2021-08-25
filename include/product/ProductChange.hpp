#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>


class OrderBook;

class ProductChange{
public:
    ProductChange(const std::string& productId, long unsigned sequenceNumber): 
        m_productId(productId),
        m_sequenceNumber(sequenceNumber){}
    virtual std::string getProductId() const {return m_productId;}
    long unsigned getSequenceNumber() const {return m_sequenceNumber;}
    
    virtual bool updateOrderBook(OrderBook* orderBook) const = 0;
    virtual ~ProductChange(){};

private:
    std::string m_productId;
    unsigned long m_sequenceNumber;
};