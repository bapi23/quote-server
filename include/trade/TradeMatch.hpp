#pragma once
#include <string>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"
#include "Message.pb.h"

class TradeMatch: public Trade{
public:
    TradeMatch(std::string orderId, FloatingP size, const std::string& productId):
        Trade(productId),
        m_orderId(orderId),
        m_size(size)
    {
    }
    
    std::string orderId(){
        return m_orderId;
    }

    std::string generateMessage(){
        qs::qsMessage pMessage;
        pMessage.set_message_type(qs::MessageType_TRADE);

        qs::Trade* trade = pMessage.mutable_trade();
        trade->set_type(qs::TradeType_MATCH);
        trade->set_order_id(m_orderId);
        trade->set_product_id(getProductId());
        trade->set_size(m_size);

        return pMessage.SerializeAsString();
    }

private:
    std::string m_orderId;
    FloatingP m_size;
};