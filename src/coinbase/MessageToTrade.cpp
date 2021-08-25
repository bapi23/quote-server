#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "coinbase/MessageToTrade.hpp"

#include "coinbase/CoinbaseFeedMessageHandler.hpp"

#include "trade/TradeActivate.hpp"
#include "trade/TradeMatch.hpp"
#include "trade/TradeReceive.hpp"
#include "Side.hpp"


std::unique_ptr<Trade> MessageToTrade::getTrade(const nlohmann::json& jmsg) {
    std::unique_ptr<Trade> trade;

    if(jmsg["type"].get<std::string>() == "activate"){
            const std::string orderId = jmsg["order_id"].get<std::string>();
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["size"].get<std::string>();
            const std::string stop_type = jmsg["stop_type"].get<std::string>();
            const std::string stop_price = jmsg["stop_price"].get<std::string>();
            const std::string founds = jmsg["founds"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            const std::string productId =  jmsg["product_id"].get<std::string>();
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " id: " << orderId << std::endl;
            trade = std::make_unique<TradeActivate>(orderId, std::stod(size), std::stod(price), std::stod(founds), std::stod(stop_price), stop_type, productId);
        } else if (jmsg["type"].get<std::string>() == "received") {
            const std::string orderId = jmsg["order_id"].get<std::string>();
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            const std::string founds = jmsg["founds"].get<std::string>();
            const std::string orderType = jmsg["order_type"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            const std::string productId =  jmsg["product_id"].get<std::string>();

            trade = std::make_unique<TradeReceive>(orderId, std::stod(founds), side, orderType, productId);
        } else if (jmsg["type"].get<std::string>() == "match") {
            const std::string makerOrderId = jmsg["maker_order_id"].get<std::string>();
            const std::string takerOrderId = jmsg["taker_order_id"].get<std::string>();
            long unsigned sequence = jmsg["sequence"].get<long unsigned>();
            const std::string price = jmsg["price"].get<std::string>();
            const std::string size = jmsg["size"].get<std::string>();
            const std::string productId =  jmsg["product_id"].get<std::string>();
            const Side side = jmsg["side"].get<std::string>() == "sell"? Side::Sell: Side::Buy;
            //std::cout << "[FEED]:" << jmsg["type"].get<std::string>() << " maker id: " << makerOrderId << " taker id: " << takerOrderId <<std::endl;
            trade = std::make_unique<TradeMatch>(takerOrderId, std::stod(size), productId); // TODO FIX!
        } else {
            std::cout << "Not a trade message" << jmsg;
        }
    return trade;
}