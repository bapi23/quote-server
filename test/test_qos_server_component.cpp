#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <iostream>

#include "coinbase/CoinbaseFeedClient.hpp"
#include "Market.hpp"
#include "OrderBookPublisherFactoryMock.hpp"
#include "ProductChangeResetOrderBook.hpp"

#include "FeedClientSpy.hpp"

namespace{

}

TEST_CASE("SubscribedShouldAddProductListener", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<OrderBookPublisherFactoryMock> publisherFactory = std::make_unique<OrderBookPublisherFactoryMock>();
    OrderBookPublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);
}

TEST_CASE("SubscribedShouldPublishTheData", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<OrderBookPublisherFactoryMock> publisherFactory = 
                            std::make_unique<OrderBookPublisherFactoryMock>();
    OrderBookPublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);

    std::vector<Order> bids = {Order{4.3, 4.5, "1"}, Order{4.3, 4.5, "2"}};
    std::vector<Order> asks = {Order{4.3, 4.5, "1"}, Order{4.3, 4.5, "2"}};
    auto prodChangReq = 
        std::make_unique<ProductChangeResetOrderBook>(bids, asks);
    feedPtr->m_listeners["ETH-USD"]->onProductChange(std::move(prodChangReq));

    REQUIRE(publisherFactoryPtr->pubPtr->messages.size() == 1);
    REQUIRE(publisherFactoryPtr->pubPtr->messages[0] == "{'bids':{}, asks{}}");
}