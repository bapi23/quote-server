#pragma once
#include <string>
#include <nlohmann/json.hpp>

#include "Trade.hpp"
#include "utils/FloatingP.hpp"

class TradeChange: public Trade{
public:
    TradeChange(const std::string& order_id, FloatingP newSize){

    }

    std::string orderId(){
        return m_orderId;
    }
    
    virtual std::string generateMessage(){
        nlohmann::json jmessage =
        {"trade", {
                {"type", "change"},
                {"order_id", m_orderId},
                {"new_size", newSize}
            }
        };
        return jmessage.dump();
    }
private:
    std::string m_orderId;
    FloatingP newSize;
};