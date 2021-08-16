#pragma once
#include <string>

#include "Trade.hpp"

class TradeActivate: public Trade{
public:
    TradeActivate(const std::string& orderId, FloatingP size, FloatingP price):
        m_orderId(orderId),
        m_size(size),
        m_price(price)
    {
    }

    std::string orderId(){
        return m_orderId;
    }
    
    virtual std::string generateMessage(){
        nlohmann::json jmessage = {
            {"message", "trade"}, 
            {"type", "activate"},
            {"order_id", m_orderId},
            {"size", m_size},
            {"price", m_price}
        };
        return jmessage.dump();
    }

    private:
    std::string m_orderId;
    FloatingP m_size;
    FloatingP m_price;
};