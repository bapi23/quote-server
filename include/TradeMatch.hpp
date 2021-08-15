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
        nlohmann::json jmessage = {
            {"message", "trade"},
            {"type", "match"},
            {"order_id", m_orderId},
            {"new_size", m_size}
        };
        return jmessage.dump();
    }

private:
    std::string m_orderId;
    FloatingP m_size;
};