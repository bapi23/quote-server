#include <string>
#include <unordered_map>
#include <vector>

#include "MarkedChangeListener.hpp"
#include "MarketChange.hpp"
#include "Client.hpp"

class OrderBookStorage: public MarketChangeListener{
public:
    void onMarketChange(std::unique_ptr<MarketChange> mc) override {
        std::vector<std::string> changedProductIds = mc->updateOrderBooks(orderBooks);
        std::vector<Trade> trades = mc->getTrades();

        for(const auto orderBookId: changedProductIds){
            auto found = clients.find(orderBookId);
            if(found != clients.end()){
                auto& clients = found->second;
                for(auto& client: clients){
                    client->onOrderBook(orderBooks.at(orderBookId));
                }
            }
        }

        for(auto trade: trades){
            auto found = clients.find(trade.getProductId());
            if(found != clients.end()){
                auto& clients = found->second;
                for(auto& client: clients){
                    client->onTrade(trade);
                }
            }
        }
    }

private:
    std::unordered_map<std::string, OrderBook> orderBooks;
    std::unordered_map<std::string, std::vector<Client*>> clients;
}