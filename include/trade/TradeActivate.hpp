#pragma once
#include <string>

#include "Trade.hpp"
#include "Message.pb.h"

class TradeActivate: public Trade{
public:
    TradeActivate(const std::string& orderId, 
                    FloatingP size, 
                    FloatingP price,
                    FloatingP founds,
                    FloatingP stopPrice,
                    const std::string stopType,
                    const std::string& productId):
        Trade(productId),
        m_orderId(orderId),
        m_size(size),
        m_price(price),
        m_founds(founds),
        m_stopPrice(stopPrice),
        m_stopType(stopType)
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
        trade->set_product_id(getProductId());
        trade->set_size(m_size);
        trade->set_price(m_price);
        trade->set_founds(m_founds);
        trade->set_stop_price(m_stopPrice);
        trade->set_stop_type(m_stopType);

        return pMessage.SerializeAsString();
    }

    private:
    std::string m_orderId;
    FloatingP m_size;
    FloatingP m_price;
    FloatingP m_founds;
    FloatingP m_stopPrice;
    std::string m_stopType;
};