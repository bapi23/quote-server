#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "Trade.hpp"

class OrderBook;

class MarketChange{
public:
    virtual std::vector<Trade> getTrades() = 0;
    virtual std::vector<std::string> updateOrderBooks(const std::unordered_map<std::string, OrderBook>& orderBooks) = 0;
    virtual ~MarketChange(){};
};