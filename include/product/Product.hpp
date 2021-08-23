#pragma once

#include <memory>
#include <vector>

#include "Client.hpp"
#include "ProductChangeListener.hpp"
#include "product/ProductChangePublisher.hpp"
#include "OrderBook.hpp"
#include "FeedClient.hpp"
#include "Client.hpp"

class Product {
public:
Product(const std::string productId, 
        std::unique_ptr<ProductChangePublisher> publisher): 
    m_productId(productId),
    m_orderBook(std::make_unique<OrderBook>()),
    m_publisher(std::move(publisher))
{

}

void onProductChange(std::unique_ptr<ProductChange> pc) {
 
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    bool bookUpdated = pc->updateOrderBook(m_orderBook.get());
    std::vector<std::unique_ptr<Trade>> trades = std::move(pc->getTrades());
    if(!trades.empty()){
        for(auto& trade: trades){
            m_publisher->publish(std::move(trade));
        }
    }
    if(bookUpdated){
        m_publisher->publish(m_orderBook.get());
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto difference = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Updating and publishing took" << difference << " micro seconds" << std::endl;
}

int hasClients(){
    return m_clients.size();
}

void appendClient(Client* client){
    m_clients[client->id()] = client;
}

void removeClient(const std::string& clientId){
    m_clients.erase(clientId);
}

private:
    std::string m_productId;
    std::unordered_map<std::string, Client*> m_clients;
    std::unique_ptr<OrderBook> m_orderBook;
    std::unique_ptr<ProductChangePublisher> m_publisher;
};