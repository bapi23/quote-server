#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <iostream>

#include "coinbase/CoinbaseFeedClient.hpp"
#include "Market.hpp"
#include "ProductChangePublisherFactoryMock.hpp"
#include "ProductChangeResetOrderbook.hpp"

#include "FeedClientSpy.hpp"

TEST_CASE("SubscribedShouldAddProductListener", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<ProductChangePublisherFactoryMock> publisherFactory = std::make_unique<ProductChangePublisherFactoryMock>();
    ProductChangePublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);
}

TEST_CASE("SubscribedShouldPublishTheData", "MarketTest") {
    auto feed = std::make_unique<FeedClientSpy>();
    FeedClientSpy* feedPtr = feed.get();
    std::unique_ptr<ProductChangePublisherFactoryMock> publisherFactory = 
                            std::make_unique<ProductChangePublisherFactoryMock>();
    ProductChangePublisherFactoryMock* publisherFactoryPtr = publisherFactory.get();
    Market market(std::move(feed), std::move(publisherFactory));
    market.subscribe("clientId", "ETH-USD");
    REQUIRE(feedPtr->m_listeners.size() == 1);

    auto prodChangReq = 
        std::make_unique<ProductChangeResetOrderbook>({Order{4.3, 4.5, "1"}}, {Order{2.3, 4.5, "2"}})
    feedPtr->m_listeners[0]->onProductChange(std::move(prodChangReq));

    REQUIRE(publisherFactoryPtr->pubPtr->messages.size() == 1);
}