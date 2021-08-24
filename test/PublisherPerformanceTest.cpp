#define CATCH_CONFIG_MAIN
#include <iostream>
#include <thread>
#include <vector>

#include "catch2/catch.hpp"


#include "coinbase/CoinbaseFeedClient.hpp"
#include "product/ProductChangePublisherZMQ.hpp"
#include "RestTransport.hpp"
#include "OrderBook.hpp"


TEST_CASE("ZMQPublisherPerformanceTest", "ZMQPublisherTest") {
    std::unique_ptr<OrderBook> book = std::make_unique<OrderBook>();
    for(int i = 0; i < 100000; ++i){
        book->addOrder(Order{5.5, 6.7, std::to_string(i)}, Side::Buy);
    }
    
    ProductChangePublisherZMQ publisher("ETH-USD");
    publisher.publish(book.get());
}