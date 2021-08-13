#pragma once

#include <memory>
#include <vector>

#include "Client.hpp"
#include "ProductChangeListener.hpp"
#include "OrderBookPublisher.hpp"
#include "OrderBook.hpp"
#include "FeedClient.hpp"
#include "Client.hpp"

class Product: public ProductChangeListener{
public:
Product(const std::string productId, 
        std::unique_ptr<OrderBookPublisher> publisher): 
    m_productId(productId), 
    m_publisher(std::move(publisher)),
    m_orderBook(std::make_unique<OrderBook>()){

}

~Product(){
    feedClient->unsubscribe(m_productId);
}

void onProductChange(std::unique_ptr<ProductChange> pc) override {
    std::vector<std::unique_ptr<Trade>> trades = std::move(pc->getTrades());
    bool bookUpdated = pc->updateOrderBook(m_orderBook.get());
    if(!trades.empty()){
        for(const auto& trade: trades){
            std::cout << "Publishing trade" << std::endl;
            m_publisher->publish(trade->generateMessage());
        }
    }
    if(bookUpdated){
        std::cout << "Publishing order book" << std::endl;
        m_publisher->publish(m_orderBook.get());
    }
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
    FeedClient* feedClient;
    std::unique_ptr<OrderBookPublisher> m_publisher;
};