#pragma once
#include <unordered_map>

namespace transport{
    std::unordered_map<std::string, std::string> prodIdToPort = {
        {"ETH-USD", "8000"},
        {"BTC-USD", "8001"},
        {"BCC-USD", "8002"}
    };
}