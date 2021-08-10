#pragma once
#include <string>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"

class TradeMatch: public Trade{
public:
    TradeMatch(std::string orderId, FloatingP size):
        m_orderId(orderId),
        m_size(size)
    {
    }
    
    std::string orderId(){
        return m_orderId;
    }

    std::string generateMessage(){
        return "{}";
    }

private:
    std::string m_orderId;
    FloatingP m_size;
};