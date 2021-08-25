
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

void Market::stop(){
    isRunning = false;
    if(m_productChangeHandlerThread.joinable()){
        m_productChangeHandlerThread.join();
    }
}

Market::~Market(){
    isRunning = false;
    if(m_productChangeHandlerThread.joinable()){
        m_productChangeHandlerThread.join();
    }
}

void Market::onProductChange(std::unique_ptr<ProductChange> pc)
{
    assert(pc);
    {
        std::lock_guard<std::mutex> lg(productChangeHandlerMutex);
        productChanges.insert({pc->getProductId(), std::move(pc)});
    }
    productChangeCv.notify_one();
}

void Market::onTrade(std::unique_ptr<Trade> trade){
    std::lock_guard<std::mutex> lg(mutexTrades);
    auto it = m_trade_publishers.find(trade->getProductId());
    if(it == m_trade_publishers.end()){
        std::cout << "Received trade without any client" << std::endl;
        return;
    }
    it->second->publish(std::move(trade));
}

void Market::processProductChanges(){
    using namespace std::chrono_literals;
    while(isRunning){
        std::unordered_multimap<std::string, std::unique_ptr<ProductChange>> currentChanges;
        {
            std::unique_lock<std::mutex> lk1(productChangeHandlerMutex);
            productChangeCv.wait_for(lk1, 100ms, [this]{return !productChanges.empty();});
            currentChanges = std::move(productChanges);
            productChanges.clear();
        }

        std::unique_lock<std::mutex> lk2(marketDataMutex);
        auto it = currentChanges.begin();
        while(it != currentChanges.end()){
            {
                const std::string productId = it->first;
                std::vector<std::unique_ptr<ProductChange>> changes;
                while(it != currentChanges.end() && it->first == productId){
                    changes.push_back(std::move(it->second));
                    ++it;
                }

                auto prodIt = m_products.find(productId);
                if(prodIt != m_products.end()){
                    prodIt->second->onProductChanges(std::move(changes));
                } else {
                    std::cout << "No product with" << productId << " id" << std::endl;
                }
            }
        }
    }
}

void Market::subscribe(const std::string& clientId, const std::string& prodId) {
    {
        std::lock_guard<std::mutex> lg(marketDataMutex);
        std::cout << "Subscribing client " << clientId << " for " << prodId;

        auto it = m_clients.find(clientId);
        if(it == m_clients.end()){
            m_clients.insert({clientId, new Client(clientId)});
        }

        auto prodIt = m_products.find(prodId);
        if(prodIt == m_products.end()){
            m_products.insert({prodId, std::make_shared<Product>(prodId, m_publisherFactory->createPublisher(prodId))});
            m_feedClient->subscribe(prodId, this);
        }
    }
    {
        std::lock_guard<std::mutex> lg(mutexTrades);
        auto tradeIt = m_trade_publishers.find(prodId);
        if(tradeIt == m_trade_publishers.end()){
            m_trade_publishers.insert({prodId, std::make_unique<TradePublisher>(prodId)});
        }
    }
}

void Market::unsubscribe(const std::string& clientId, const std::string& prodId) {
    {
        std::scoped_lock lck{marketDataMutex, mutexTrades};
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
            m_trade_publishers.erase(prodId);
        }
    }
}
