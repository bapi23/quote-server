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


std::string generateMsg(const std::string& type,
                     const std::string& prodId,
                     const std::string& clientId,
                     const std::string& size,
                     const std::string& price,
                     const std::string& side,
                     const std::string& orderId,
                     const long unsigned seq){
  nlohmann::json msg {
    {"type", type},
    {"product_id", prodId},
    {"order_id", orderId},
    {"client_id", clientId},
    {"size", size},
    {"price", price},
    {"sequence", seq},
    {"side", side}
  };
  return msg.dump();
}

std::string doneMsg(const std::string& prodId, const std::string& clientId, const std::string& side, const std::string& orderId, long unsigned seq){
  return generateMsg("done", prodId, clientId, "100", "100", side, orderId, seq);
}

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


TEST_CASE("DoneMessageReceivedShouldAdd", "CoibaseFeedTest") {
    std::string clientId = "1";
    std::string prodId = "ETH-USD";
    
    RestTransport::clear();
    RestTransport::bids.push_back(Order{1.1, 100, "OrderNb1"});
    RestTransport::asks.push_back(Order{1.1, 100, "OrderNb2"});
    RestTransport::request_time_in_ms = 0;
    ProductListener listener;
    TradeListenerSpy tradeListener;
    {
      CoinbaseFeedClient client;
      client.setTradeListener(&tradeListener);
      client.subscribe(prodId, &listener);
      auto start = std::chrono::high_resolution_clock::now();
      std::this_thread::sleep_for(100ms);

      client.onMessageReceived(doneMsg(prodId, clientId, "sell", "OrderNb1", 2));
      std::this_thread::sleep_for(100ms);
    }
    REQUIRE(listener.productChanges.size() == 2); // full orderbook msg with full orderbook and done message

    OrderBook orderbook;
    listener.productChanges[0]->updateOrderBook(&orderbook);
    listener.productChanges[1]->updateOrderBook(&orderbook);
    REQUIRE(orderbook.getBids().size() == 0);
    REQUIRE(orderbook.getAsks().size() == 1);
}


TEST_CASE("DoneMessageReceivedShouldAddAsMerged", "CoibaseFeedTest") {
    std::string clientId = "1";
    std::string prodId = "ETH-USD";

    RestTransport::clear();
    RestTransport::bids.push_back(Order{1.1, 100, "OrderNb1"});
    RestTransport::asks.push_back(Order{1.1, 100, "OrderNb2"});
    RestTransport::request_time_in_ms = 2000;
    RestTransport::sequence = 1;
    ProductListener listener;
    TradeListenerSpy tradeListener;
    {
      CoinbaseFeedClient client;
      client.setTradeListener(&tradeListener);
      client.subscribe(prodId, &listener);
      std::this_thread::sleep_for(100ms);

      client.onMessageReceived(doneMsg(prodId, clientId, "sell", "OrderNb1", 2));
      std::this_thread::sleep_for(100ms);
    }
    REQUIRE(listener.productChanges.size() == 1); // full orderbook msg with full orderbook and done message

    OrderBook orderbook;
    listener.productChanges[0]->updateOrderBook(&orderbook);
    REQUIRE(orderbook.getBids().size() == 0);
    REQUIRE(orderbook.getAsks().size() == 1);
}