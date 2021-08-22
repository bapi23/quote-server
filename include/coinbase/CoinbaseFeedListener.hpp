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

class CoinbaseFeedListener{
public:
    CoinbaseFeedListener(const std::string productId, ProductChangeListener* listener);
    ~CoinbaseFeedListener();

    void requestFullOrderBook();
    void onMessageReceived(const nlohmann::json& message);

private:
    void requestFullOrderBookImpl();
    void messageLoop();

    CoinbaseFeedListener(CoinbaseFeedListener&& other) = delete;
    CoinbaseFeedListener& operator=(CoinbaseFeedListener&& other) = delete;
    CoinbaseFeedListener(CoinbaseFeedListener&) = delete;
    CoinbaseFeedListener& operator=(CoinbaseFeedListener&) = delete;

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