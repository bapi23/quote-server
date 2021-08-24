#pragma once
#include <string>
#include <nlohmann/json.hpp>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"
#include "Message.pb.h"

class TradeDone: public Trade{
public:
    TradeDone(const std::string& orderId):
        m_orderId(orderId)
    {}

    std::string orderId() override {
        return m_orderId;
    }

    std::string generateMessage() override {
        qs::qsMessage pMessage;
        pMessage.set_message_type(qs::MessageType_TRADE);

        qs::Trade* trade = pMessage.mutable_trade();
        trade->set_type(qs::TradeType_DONE);
        trade->set_order_id(m_orderId);

        return pMessage.SerializeAsString();
    }

    std::string m_orderId;
};