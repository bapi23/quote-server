#pragma once

#include <string>

class MessageReceiver{
public:
    virtual void onMessageReceived(const std::string& message);
    virtual ~MessageReceiver(){}
};