
#include <unordered_map>
#include <string>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <iostream>

#include "WebsocketTransport.hpp"
#include "MessageReceiver.hpp"

typedef websocketpp::client<websocketpp::config::asio_tls_client> websocket_client;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;



void WebsocketTransport::connect(const std::string& uri){
}

void WebsocketTransport::send(const std::string& msg){
}

void WebsocketTransport::subscribe(const std::string& id, MessageReceiver* receiver){
    subscribers[id] = receiver;
}

void WebsocketTransport::unsubscribe(const std::string& id){
    subscribers.erase(id);
}
