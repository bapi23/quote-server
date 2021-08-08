#pragma once

#include <string>
#include "OrderBook.hpp"
#include "Trade.hpp"

class Client{
public:
    Client(std::string clientId): m_clientId(clientId){}
    void onOrderBook(const OrderBook& orderBook){};
    void onTrade(const Trade& trade){};
    std::string id() const {
        return m_clientId;
    }
private:
    std::string m_clientId;
};