#pragma once

#include <unordered_map>
#include <string>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <iostream>

#include "MessageReceiver.hpp"

typedef websocketpp::client<websocketpp::config::asio_tls_client> websocket_client;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class WebsocketTransport{
public:
    WebsocketTransport();
    ~WebsocketTransport();

    void connect(const std::string& uri);
    void send(const std::string& msg);
    void subscribe(const std::string& id, MessageReceiver* receiver);
    void unsubscribe(const std::string& id);

private:
    WebsocketTransport(WebsocketTransport& wt) = default;
    WebsocketTransport& operator=(const WebsocketTransport& wt) = default;

    void on_message(websocketpp::connection_hdl, websocket_client::message_ptr msg);

    void on_open(websocket_client* c, websocketpp::connection_hdl hdl);

    websocket_client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    std::unordered_map<std::string, MessageReceiver*> subscribers;
};