#pragma once

#include <string>
#include <deque>
#include <sstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <nlohmann/json.hpp>


#include "CoinbaseFeedMessageHandler.hpp"
#include "WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

using json = nlohmann::json;

class CoinbaseFeedProduct{
public:
    CoinbaseFeedProduct(const std::string productId, ProductChangeListener* listener);
    ~CoinbaseFeedProduct();

    void requestFullOrderBook();
    void onMessageReceived(const nlohmann::json& message);

private:
    void requestFullOrderBookImpl();
    void messageLoop();

    CoinbaseFeedProduct(CoinbaseFeedProduct&& other) = delete;
    CoinbaseFeedProduct& operator=(CoinbaseFeedProduct&& other) = delete;
    CoinbaseFeedProduct(CoinbaseFeedProduct&) = delete;
    CoinbaseFeedProduct& operator=(CoinbaseFeedProduct&) = delete;

private:
    std::string m_productId;
    CoinbaseFeedMessageHandler m_msgHandler;
    std::deque<nlohmann::json> m_messageQueue;
    long unsigned m_lastSequenceNumber;
    std::mutex m_messagesMutex;
    std::condition_variable m_messages_cv;
    std::atomic<bool> m_isRunning{true};
    std::atomic<bool> m_requestingOrderbook{true};
    std::unique_ptr<std::thread> m_messageHandlerThread;
    std::unique_ptr<std::thread> m_fullOrderBookRequestThread;
    
};