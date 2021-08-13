#pragma once
#include <string>

#include "Trade.hpp"

class TradeChange: public Trade{
public:
    TradeChange(){

    }

    std::string orderId(){
        return "";
    }
    
    virtual std::string generateMessage(){
        return "{}";
    }
};