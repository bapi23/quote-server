#pragma once
#include <string>
#include <nlohmann/json.hpp>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"
#include "Message.pb.h"

class TradeChange: public Trade{
public:
    TradeChange(const std::string& order_id, FloatingP newSize, const std::string& productId):
        Trade(productId)
    {

    }

    std::string orderId(){
        return m_orderId;
    }
    
    virtual std::string generateMessage(){
        qs::qsMessage pMessage;
        pMessage.set_message_type(qs::MessageType_TRADE);

        qs::Trade* trade = pMessage.mutable_trade();
        trade->set_type(qs::TradeType_CHANGE);
        trade->set_order_id(m_orderId);
        trade->set_product_id(getProductId());
        trade->set_size(newSize);

        return pMessage.SerializeAsString();
    }
private:
    std::string m_orderId;
    FloatingP newSize;
};