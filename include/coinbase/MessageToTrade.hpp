#pragma once

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "trade/Trade.hpp"

class MessageToTrade{
public:
    static std::unique_ptr<Trade> getTrade(const nlohmann::json& jmsg);

};