#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "product/Product.hpp"
#include "product/ProductSubscriber.hpp"
#include "Client.hpp"
#include "FeedClient.hpp"
#include "product/ProductChangePublisherFactory.hpp"

class Market: public ProductSubscriber, ProductChangeListener{
public:
    Market(std::unique_ptr<FeedClient> feedClient, 
           std::unique_ptr<ProductChangePublisherFactory> publisherFactory);
    ~Market();

    void onProductChange(std::unique_ptr<ProductChange> pc) override;
    void processProductChanges();
    void subscribe(const std::string& clientId, const std::string& prodId) override;
    void unsubscribe(const std::string& clientId, const std::string& prodId) override;

    // for testing purposes
    void stop();

private:
    std::unique_ptr<FeedClient> m_feedClient;
    std::unordered_map<std::string, std::shared_ptr<Product>> m_products;
    std::unordered_map<std::string, Client*> m_clients;
    std::unique_ptr<ProductChangePublisherFactory> m_publisherFactory;
    std::mutex marketDataMutex;
    std::mutex productChangeHandlerMutex;
    std::unordered_multimap<std::string, std::unique_ptr<ProductChange>> productChanges;
    std::condition_variable productChangeCv;
    std::atomic<bool> isRunning{true};
    std::thread m_productChangeHandlerThread;
};
