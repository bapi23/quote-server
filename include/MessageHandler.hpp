#include <string>

class MessageHandler{
    virtual void onHandleMessage(const std::string& msg) = 0;
};