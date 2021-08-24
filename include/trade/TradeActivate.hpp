#pragma once
#include <string>

#include "Trade.hpp"
#include "Message.pb.h"

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
        qs::qsMessage pMessage;
        pMessage.set_message_type(qs::MessageType_TRADE);

        qs::Trade* trade = pMessage.mutable_trade();
        trade->set_type(qs::TradeType_ACTIVATE);
        trade->set_order_id(m_orderId);
        trade->size(m_size);
        trade->price(m_price);

        return pMessage.SerializeAsString();
    }

    private:
    std::string m_orderId;
    FloatingP m_size;
    FloatingP m_price;
};