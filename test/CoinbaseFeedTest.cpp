#define CATCH_CONFIG_MAIN
#include <iostream>
#include <vector>

#include "catch2/catch.hpp"


#include "coinbase/CoinbaseFeedClient.hpp"
#include "RestTransport.hpp"



std::string generateMsg(const std::string& type,
                     const std::string& prodId,
                     const std::string& clientId,
                     const std::string& size,
                     const std::string& price,
                     const std::string& seq){
  nlohmann::json msg {
    {"type", type},
    {"product_id", prodId},
    {"client_id", clientId},
    {"size", size},
    {"price", price},
    {"sequence", seq}
  };
  return msg.dump();
}

std::string doneMsg(const std::string& prodId, const std::string& clientId, long int seq){
  return generateMsg("done", prodId, clientId, "100", "100", std::to_string(seq));
}

class ProductListener: public ProductChangeListener{
  public:
    virtual void onProductChange(std::unique_ptr<ProductChange> pc) override {
      productChanges.push_back(std::move(pc));
    }

    std::vector<std::unique_ptr<ProductChange>> productChanges;
};


TEST_CASE("DoneMessageReceivedShouldAdd", "CoibaseFeedTest") {
    std::string clientId = "1";
    std::string prodId = "ETH-USD";

    RestTransport::bids.push_back(Order{1.1, 100, "OrderNb1"});
    RestTransport::asks.push_back(Order{1.1, 100, "OrderNb2"});
    std::cout <<  RestTransport::request("fff");
    ProductListener listener;
    {
      CoinbaseFeedClient client;
      client.subscribe(prodId, &listener);
      client.onMessageReceived(doneMsg(prodId, clientId, 123));
    }
    REQUIRE(listener.productChanges.size() == 2); // full orderbook msg + done
}