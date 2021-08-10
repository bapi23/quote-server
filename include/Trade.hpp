#pragma once
#include <string>


        // Open,
        // Done,
        // Match,
        // Change,
        // Activate

class Trade{
public:
    virtual std::string generateMessage() = 0;
    virtual std::string orderId() = 0;
    virtual ~Trade() = default;
};