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
            assert(productIds.find(prodId) != productIds.end());
            return productIds[prodId];
        }

        bool contains(const std::string& prodId){
            return productIds.find(prodId) != productIds.end();
        }

        private:
        EndpointProvider(){
            auto results = RestTransport::request("https://api.pro.coinbase.com/products/");
            auto jmsg = nlohmann::json::parse(results);
            int orderPort = 83000;
            int tradePort = 93000;
            for (const auto& productId: jmsg) {
                if(productId["status"] == "online"){
                    productIds.insert({productId["id"], Endpoint{"127.0.0.1", std::to_string(orderPort++), std::to_string(tradePort++) }});
                }
            }
        }

        std::unordered_map<std::string, Endpoint> productIds;
    };
}