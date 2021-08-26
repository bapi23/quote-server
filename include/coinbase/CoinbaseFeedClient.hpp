#pragma once

#include <vector>
#include <algorithm>
#include "nlohmann/json.hpp"
#include <boost/circular_buffer.hpp>

#include "RestTransport.hpp"
#include "WebsocketTransport.hpp"
#include "FeedClient.hpp"
#include "CoinbaseFeedProduct.hpp"
#include "TradeListener.hpp"


class CoinbaseFeedClient: public FeedClient, MessageReceiver{
public:
    CoinbaseFeedClient();
    ~CoinbaseFeedClient();

    void onMessageReceived(const std::string& message);
    void subscribe(const std::string& productId, 
                   ProductChangeListener* listener) override;
    void unsubscribe(const std::string& productId) override;
    void setTradeListener(TradeListener* listener);

private:
    std::string generateSubscribeMessage(const std::string& productId);
    std::string generateUnsubscribeMessage(const std::string& productId);

    std::unordered_map<std::string, std::unique_ptr<CoinbaseFeedProduct>> m_prodIdToListener;
    TradeListener* m_tradeListener = nullptr;
    WebsocketTransport m_feedTransport;
    bool m_connected = false;

    std::mutex m_mutexFeedData;
    boost::circular_buffer<std::chrono::steady_clock::time_point> m_stamps;
};