#pragma once
#include <nlohmann/json.hpp>

#include "RestTransport.hpp"

#include <unordered_map>

// TODO It's a "very simple" prototype of the service which provides endpoints with the product Ids
// In the production code it should be implemented like load balancing service which gives existing endpoints or creates another one
// TODO get list from https://api.pro.coinbase.com/products/

namespace transport{
    struct Endpoint{
        std::string address;
        std::string orderbookPort;
        std::string tradePort;
    };
    
    class EndpointProvider{
        public:
        static EndpointProvider& getInstance(){
            static EndpointProvider provider;
            return provider;
        }

        Endpoint prodIdToPort(const std::string& prodId){
            return productIds[prodId];
        }

        bool contains(const std::string& prodId){
            return true;
        }

        private:
        EndpointProvider(){
        }

        std::unordered_map<std::string, Endpoint> productIds;
    };
}