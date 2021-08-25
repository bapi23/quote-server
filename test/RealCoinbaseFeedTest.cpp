#define CATCH_CONFIG_MAIN
#include <iostream>
#include <thread>
#include <vector>

#include "catch2/catch.hpp"


#include "coinbase/CoinbaseFeedClient.hpp"
#include "RestTransport.hpp"
#include "OrderBook.hpp"
#include "TradeListener.hpp"

using namespace std::chrono_literals;

class ProductListener: public ProductChangeListener{
  public:
    virtual void onProductChange(std::unique_ptr<ProductChange> pc) override {
      productChanges.push_back(std::move(pc));
    }

    std::vector<std::unique_ptr<ProductChange>> productChanges;
};

class TradeListenerSpy: public TradeListener{
  public:
    virtual void onTrade(std::unique_ptr<Trade> trade) override {
      trades.push_back(std::move(trade));
    }

    std::vector<std::unique_ptr<Trade>> trades;
};

struct Result{
  std::vector<Order> bids;
  std::vector<Order> asks;
  long unsigned seq;
};

std::vector<Result> fullOrderbookRequests;
std::atomic<bool> requestOrderBook{true};

void requestInLoop(){
  while(requestOrderBook){
    auto result = RestTransport::request("https://api.pro.coinbase.com/products/ETH-BTC/book?level=3");
    auto jmsg = nlohmann::json::parse(result);

    std::vector<Order> bids;
    std::vector<Order> asks;

    for(const auto& order: jmsg["asks"]){
        const std::string price = order[0].get<std::string>();
        const std::string size = order[1].get<std::string>();
        const std::string orderId = order[2].get<std::string>();
        asks.push_back(Order{std::stod(price), std::stod(size), orderId});
    }
    for(const auto& order: jmsg["bids"]){
        const std::string price = order[0].get<std::string>();
        const std::string size = order[1].get<std::string>();
        const std::string orderId = order[2].get<std::string>();
        bids.push_back(Order{std::stod(price), std::stod(size), orderId});
    }

    long unsigned bookSeq = jmsg["sequence"].get<long unsigned>();
    std::cout << "RECEIVED " << bookSeq << std::endl;
    Result res{bids, asks, bookSeq};

    fullOrderbookRequests.push_back(res);
  }
}


TEST_CASE("MatchOrderbookWithRESTRequest", "ProductionCoinbaseFeedTest") {
    std::string clientId = "1";
    std::string prodId = "ETH-BTC";
    
    ProductListener listener;
    TradeListenerSpy tradeListener;
    std::thread orderBookThread(&requestInLoop);
    {
      CoinbaseFeedClient client;
      client.setTradeListener(&tradeListener);
      client.subscribe(prodId, &listener);
      std::this_thread::sleep_for(5000ms);
    }
    
    std::this_thread::sleep_for(10000ms);
    requestOrderBook = false;
    orderBookThread.join();

    OrderBook book;

      for(auto& productChange: listener.productChanges){
        productChange->updateOrderBook(&book);
        auto pcSeq = productChange->getSequenceNumber();

        for(const auto& fullBook: fullOrderbookRequests){
          auto bookSeq = fullBook.seq;
          if(pcSeq == bookSeq){
            std::cout << "Matching " << pcSeq << " sequence" << std::endl;
            std::vector<std::string> realIds;
            std::vector<std::string> bookIds;

            REQUIRE(book.getAsks().size() == fullBook.asks.size());
            for(const auto& ask: book.getAsks()){
              bookIds.push_back(ask.order_id);
            }
            for(const auto& ask: fullBook.asks){
              realIds.push_back(ask.order_id);
            }
            std::sort(std::begin(realIds),std::end(realIds));
            std::sort(std::begin(bookIds), std::end(bookIds));
          }
        }
    }
}