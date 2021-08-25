#pragma once
#include <string>


        // Open,
        // Done,
        // Match,
        // Change,
        // Activate

class Trade{
public:
    Trade(const std::string& productId):
        m_productId(productId){
    }

    std::string getProductId(){
        return m_productId;
    }

    virtual std::string generateMessage() = 0;
    virtual std::string orderId() = 0;
    virtual ~Trade() = default;

private:
    std::string m_productId;
};