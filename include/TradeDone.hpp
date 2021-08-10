#pragma once
#include <string>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"

class TradeDone: public Trade{
public:
    TradeDone(const std::string& orderId):
        m_orderId(orderId)
    {}

    std::string orderId() override {
        return "";
    }

    std::string generateMessage() override {
        return "{}";
    }

    std::string m_orderId;
};