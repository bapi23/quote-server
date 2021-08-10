#pragma once
#include <string>

#include "Trade.hpp"

class TradeOpen: public Trade{
public:
    TradeOpen(){

    }
    virtual std::string generateMessage(){
        return "{}";
    }
};