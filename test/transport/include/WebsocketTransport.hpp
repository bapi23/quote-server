#pragma once

#include <unordered_map>
#include <string>

#include "MessageReceiver.hpp"

class WebsocketTransport{
public:
    void connect(const std::string& uri);
    void send(const std::string& msg);
    void subscribe(const std::string& id, MessageReceiver* receiver);
    void unsubscribe(const std::string& id);

private:
    std::unordered_map<std::string, MessageReceiver*> subscribers;
};