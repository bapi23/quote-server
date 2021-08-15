#pragma once
#include <string>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"

class TradeOpen: public Trade{
public:
    TradeOpen(const std::string& orderId, FloatingP size, FloatingP price):
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
            {"type", "open"},
            {"order_id", m_orderId},
            {"price", m_orderId},
            {"size", newSize}
        };
        return jmessage.dump();
    }

private:
    std::string m_orderId;
    FloatingP m_size;
    FloatingP m_price;

};