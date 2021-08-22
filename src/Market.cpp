
#include <vector>
#include <queue>
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "Market.hpp"
#include "product/Product.hpp"
#include "product/ProductSubscriber.hpp"
#include "Client.hpp"
#include "FeedClient.hpp"
#include "product/ProductChangePublisherFactory.hpp"


Market::Market(std::unique_ptr<FeedClient> feedClient, 
        std::unique_ptr<ProductChangePublisherFactory> publisherFactory):
        m_feedClient(std::move(feedClient)),
        m_publisherFactory(std::move(publisherFactory)),
        m_productChangeHandlerThread(std::bind(&Market::processProductChanges, this))
{
}

void Market::onProductChange(std::unique_ptr<ProductChange> pc)
{
    {
        std::lock_guard<std::mutex> lg(productChangeHandlerMutex);
        productChanges.push_back(std::move(pc));
    }
    productChangeCv.notify_one();
}

void Market::processProductChanges(){
    while(isRunning){
        std::deque<std::unique_ptr<ProductChange>> currentChanges;
        {
            std::unique_lock<std::mutex> lk1(productChangeHandlerMutex);
            productChangeCv.wait(lk1, [this]{return !productChanges.empty();});
            currentChanges = std::move(productChanges);
            productChanges.clear();
        }

        while(!currentChanges.empty()){
            auto pc = std::move(currentChanges.front());
            currentChanges.pop_front();

            //std::cout << "size of the products: " << m_products.size() << std::endl; 
            {
                std::unique_lock<std::mutex> lk2(marketDataMutex);
                auto prodIt = m_products.find(pc->getProductId());
                if(prodIt != m_products.end()){
                    prodIt->second->onProductChange(std::move(pc));
                } else {
                    std::cout << "No product with" << pc->getProductId() << " id" << std::endl;
                }
            }
        }
    }
}

void Market::subscribe(const std::string& clientId, const std::string& prodId) {
    std::lock_guard<std::mutex> lg(marketDataMutex);
    std::cout << "Subscribing client " << clientId << " for " << prodId;

    auto it = m_clients.find(clientId);
    if(it == m_clients.end()){
        m_clients.insert({clientId, new Client(clientId)});
    }

    auto prodIt = m_products.find(prodId);
    if(prodIt == m_products.end()){
        m_products.insert({prodId, std::make_shared<Product>(prodId, m_publisherFactory->createPublisher(prodId))});
    }

    m_feedClient->subscribe(prodId, this);
}

void Market::unsubscribe(const std::string& clientId, const std::string& prodId) {
    std::lock_guard<std::mutex> lg(marketDataMutex);
    std::cout << "Unsubscribing client " << clientId << " from " << prodId;
    auto it = m_products.find(prodId);
    if(it != m_products.end()){
        it->second->removeClient(clientId);
    }
    if(!it->second->hasClients())
    {
        std::cout << "No more clients for " << prodId << " unsubscribing from the Market" << std::endl;
        m_feedClient->unsubscribe(prodId);
        m_products.erase(prodId);
    }
}
