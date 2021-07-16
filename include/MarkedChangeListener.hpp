#include <memory>

#include "MarketChange.hpp"

class OrderBook;

class MarketChangeListener{
public:
    virtual void onMarketChange(std::unique_ptr<MarketChange>) = 0;
    virtual ~MarketChangeListener(){};
};