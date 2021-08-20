#pragma once

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "product/ProductChange.hpp"
#include "product/ProductChangeListener.hpp"

class CoinbaseFeedMessageHandler{
public:
    CoinbaseFeedMessageHandler(ProductChangeListener* listener, const std::string& productId);
    void onMessageReceived(const nlohmann::json& jmsg);

private:
    std::unique_ptr<ProductChange> getProductChange(const nlohmann::json& jmsg);

    ProductChangeListener* m_listener;
    std::string m_productId;
};