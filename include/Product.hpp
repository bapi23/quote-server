#pragma once

#include <memory>
#include <vector>

#include "Client.hpp"
#include "ProductChangeListener.hpp"
#include "OrderBook.hpp"
#include "FeedClient.hpp"
#include "Client.hpp"

class Product: public ProductChangeListener{
public:
Product(const std::string productId): m_productId(productId){

}

~Product(){
    feedClient->unsubscribe(m_productId);
}

void onProductChange(std::unique_ptr<ProductChange> pc) override {
    std::vector<std::unique_ptr<Trade>> trades = std::move(pc->getTrades());
    bool bookUpdated = pc->updateOrderBook(m_orderBook);
    if(!trades.empty()){
        for(auto& client: m_clients){
            //@TODO if would receive client id this algorithm might be improved by sending changes only to apropriate clients (not to all)
            //client.onTrade(trade)
        }
    }
    if(bookUpdated){
        for(auto& client: m_clients){
            //@TODO id fw would receive client id this algorithm might be improved by sending changes only to apropriate clients (not to all)
            //client.onOrderBook(trade)
        }
    }
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
    OrderBook m_orderBook;
    FeedClient* feedClient;
};