#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

#include "Product.hpp"
#include "ProductSubscriber.hpp"
#include "Client.hpp"
#include "FeedClient.hpp"
#include "ProductChangePublisherFactory.hpp"

class Market: public ProductSubscriber{
public:
    Market(std::unique_ptr<FeedClient> feedClient, 
           std::unique_ptr<ProductChangePublisherFactory> publisherFactory):
        m_feedClient(std::move(feedClient)),
        m_publisherFactory(std::move(publisherFactory))
    {
    }
    void subscribe(const std::string& clientId, const std::string& prodId) override {
        std::lock_guard<std::mutex> lg(marketDataMutex);

        auto it = m_clients.find(clientId);
        if(it == m_clients.end()){
            m_clients.insert({clientId, new Client(clientId)});
        }

        auto prodIt = m_products.find(prodId);
        if(prodIt == m_products.end()){
            m_products.insert({prodId, new Product(prodId, m_publisherFactory->createPublisher(prodId))});
        }

        m_feedClient->subscribe(prodId, m_products[prodId]);
    }

    void unsubscribe(const std::string& clientId, const std::string& prodId) override {
        std::lock_guard<std::mutex> lg(marketDataMutex);
        auto it = m_products.find(prodId);
        if(it != m_products.end()){
            it->second->removeClient(clientId);
        }
        if(!it->second->hasClients())
        {
            std::cout << "No more clients for " << prodId << " unsubscribing";
            m_feedClient->unsubscribe(prodId);
        }
    }

private:
    std::unordered_map<std::string, Product*> m_products;
    std::unordered_map<std::string, Client*> m_clients;
    std::unique_ptr<FeedClient> m_feedClient;
    std::unique_ptr<ProductChangePublisherFactory> m_publisherFactory;

    std::mutex marketDataMutex;
};