#pragma once
#include <string>
#include <optional>

#include "utils/FloatingP.hpp"

#include "Trade.hpp"
#include "Message.pb.h"
#include "Side.hpp"

// {
//     "type": "received",
//     "time": "2014-11-07T08:19:27.028459Z",
//     "product_id": "BTC-USD",
//     "sequence": 10,
//     "order_id": "d50ec984-77a8-460a-b958-66f114b0de9b",
//     "size": "1.34",
//     "price": "502.1",
//     "side": "buy",
//     "order_type": "limit"
// }

class TradeReceive: public Trade{
public:
    TradeReceive(const std::string& orderId, 
                  std::optional<FloatingP> size, 
                  std::optional<FloatingP> founds, 
                  std::optional<FloatingP> price, 
                  Side side,
                  const std::string orderType,
                  const std::string& productId):
        Trade(productId),
        m_orderId(orderId),
        m_founds(founds),
        m_size(size),
        m_price(price),
        m_side(side),
        m_orderType(orderType)
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
        if(m_size){
            trade->set_size(*m_size);
        }
        if(m_price){
            trade->set_price(*m_price);
        }
        if(m_price){
            trade->set_founds(*m_founds);
        }
        trade->set_order_type(m_orderType);

        return pMessage.SerializeAsString();
    }

    private:
    std::string m_orderId;
    std::optional<FloatingP> m_size;
    std::optional<FloatingP> m_price;
    std::optional<FloatingP> m_founds;
    Side m_side;
    std::string m_orderType;
};