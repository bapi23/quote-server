#include <vector>
#include <string>
#include <unordered_map>

#include "Product.hpp"
#include "Client.hpp"
#include "FeedClient.hpp"

class Market{
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
        m_feedClient->unsubscribe(prodId, clientId);
    }

private:
    std::unordered_map<std::string, Product*> m_products;
    std::unordered_map<std::string, Client*> m_clients;
    FeedClient* m_feedClient;
};