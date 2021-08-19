#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <iostream>

#include "coinbase/CoinbaseFeedClient.hpp"
#include "Market.hpp"
#include "ProductChangePublisherFactoryMock.hpp"
#include "product/ProductChangeResetOrderBook.hpp"

#include "FeedClientSpy.hpp"


bool operator == (const std::vector<Order>& lhs, const std::vector<Order>& rhs)
{
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}



TEST_CASE("SubscribedShouldAddProductListener", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<ProductChangePublisherFactoryMock> publisherFactory = std::make_unique<ProductChangePublisherFactoryMock>();
    ProductChangePublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);
}

TEST_CASE("ProductShouldPublishFullOrderbook", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<ProductChangePublisherFactoryMock> publisherFactory = 
                            std::make_unique<ProductChangePublisherFactoryMock>();
    ProductChangePublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);

    std::vector<Order> bids = {Order{4.3, 4.5, "1"}, Order{4.3, 4.5, "2"}};
    std::vector<Order> asks = {Order{4.3, 4.5, "1"}, Order{4.3, 4.5, "2"}};
    auto prodChangReq = 
        std::make_unique<ProductChangeResetOrderBook>(bids, asks, std::vector<std::unique_ptr<ProductChange>>(), "ETH-USD");
    feedPtr->m_listeners["ETH-USD"]->onProductChange(std::move(prodChangReq));

    REQUIRE(publisherFactoryPtr->pubPtr->m_asks.size() == 2);
    REQUIRE(publisherFactoryPtr->pubPtr->m_bids.size() == 2);

    REQUIRE(publisherFactoryPtr->pubPtr->m_asks == asks);
}


TEST_CASE("ProductShouldPublishTrade", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<ProductChangePublisherFactoryMock> publisherFactory = 
                            std::make_unique<ProductChangePublisherFactoryMock>();
    ProductChangePublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);

    auto prodChangReq = 
        std::make_unique<ProductChangeDone>("order_id", Side::Buy, "ETH-USD");
    feedPtr->m_listeners["ETH-USD"]->onProductChange(std::move(prodChangReq));

    REQUIRE(publisherFactoryPtr->pubPtr->m_trades.size() == 1);

    REQUIRE(publisherFactoryPtr->pubPtr->m_trades[0]->generateMessage() != "");
}