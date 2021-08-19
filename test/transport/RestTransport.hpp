#pragma once

#include <sstream>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "Order.hpp"

void to_json(nlohmann::json& j, const Order& o) {
    nlohmann::json message;
    message.push_back(std::to_string(o.price));
    message.push_back(std::to_string(o.size));
    message.push_back(o.order_id);
    j = message;
}

void from_json(const nlohmann::json& j, Order& o) {
    j[0].get_to(o.order_id);
    j[0].get_to(o.size);
    j[0].get_to(o.price);
}

class RestTransport{
public:
    static std::string request(const std::string& address){
        nlohmann::json message;

        message["sequence"] = 1;
        for(const auto& bid: bids){
            message["bids"].push_back(bid);
        }
        for(const auto& ask: asks){
            message["asks"].push_back(ask);
        }
        return message.dump();
    }

    static int sequence;
    static std::vector<Order> bids;
    static std::vector<Order> asks;
};

int RestTransport::sequence = 1;
std::vector<Order> RestTransport::bids;
std::vector<Order> RestTransport::asks;