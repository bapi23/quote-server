#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Product.hpp"
#include "Client.hpp"
#include "FeedClient.hpp"

class Market{
public:
    Market(FeedClient* feedClient){
        m_feedClient = feedClient;
    }

    void addClient(const std::string& clientId){
        
    }

    void subscribe(const std::string& clientId, const std::string& prodId){
        auto it = m_clients.find(clientId);
        if(it == m_clients.end()){
            m_clients.insert({clientId, new Client(clientId)});
        }

        auto prodIt = m_products.find(prodId);
        if(prodIt == m_products.end()){
            m_products.insert({prodId, new Product(prodId)});
        }

        m_feedClient->subscribe(prodId, m_products[prodId]);
    }

    void unsubscribe(const std::string& clientId, const std::string& prodId){
        // @TODO if no other client which is subscribed on such product:
        m_feedClient->unsubscribe(prodId);
    }

private:
    std::unordered_map<std::string, Product*> m_products;
    std::unordered_map<std::string, Client*> m_clients;
    FeedClient* m_feedClient;
};