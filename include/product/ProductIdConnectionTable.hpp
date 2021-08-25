#pragma once
#include <unordered_map>

namespace transport{
    struct Endpoint{
        std::string address;
        std::string orderbookPort;
        std::string tradePort;
    };
    
    extern std::unordered_map<std::string, Endpoint> prodIdToPort;
}