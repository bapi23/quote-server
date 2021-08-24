
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include "nlohmann/json.hpp"

#include "RestTransport.hpp"
#include "Order.hpp"

using namespace std::chrono_literals;

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

std::string RestTransport::request(const std::string& address){
    nlohmann::json message;

    std::this_thread::sleep_for(std::chrono::milliseconds(request_time_in_ms));

    message["sequence"] = 1;
    for(const auto& bid: bids){
        message["bids"].push_back(bid);
    }
    for(const auto& ask: asks){
        message["asks"].push_back(ask);
    }
    return message.dump();
}

void RestTransport::clear(){
    bids.clear();
    asks.clear();
}

int RestTransport::sequence = 1;
int RestTransport::request_time_in_ms = 2000;
std::vector<Order> RestTransport::bids;
std::vector<Order> RestTransport::asks;