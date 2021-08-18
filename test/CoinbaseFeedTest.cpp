#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <iostream>

#include "coinbase/CoinbaseFeedClient.hpp"
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
    
}