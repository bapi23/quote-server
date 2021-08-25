#include <unordered_map>

#include "RestTransport.hpp"
#include "product/ProductIdConnectionTable.hpp"
namespace transport{
    std::unordered_map<std::string, Endpoint> prodIdToPort = {
        {"ETH-USD", Endpoint{"127.0.0.1", "80000", "90000"}},
        {"BTC-USD", Endpoint{"127.0.0.1", "80001", "90001"}},
        {"BCC-USD", Endpoint{"127.0.0.1", "80002", "90002"}}
    };
}