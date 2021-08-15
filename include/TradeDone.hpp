#pragma once
#include <string>
#include <nlohmann/json.hpp>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"

class TradeDone: public Trade{
public:
    TradeDone(const std::string& orderId):
        m_orderId(orderId)
    {}

    std::string orderId() override {
        return m_orderId;
    }

    std::string generateMessage() override {
        nlohmann::json jmessage ={
            {"message", "trade"},
            {"type", "done"},
            {"order_id", m_orderId}
        };
        return jmessage.dump();
    }

    std::string m_orderId;
};