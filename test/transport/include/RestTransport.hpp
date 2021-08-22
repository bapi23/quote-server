#pragma once

#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "Order.hpp"
#include "nlohmann/json.hpp"

using namespace std::chrono_literals;

void to_json(nlohmann::json& j, const Order& o);
void from_json(const nlohmann::json& j, Order& o);

class RestTransport{
public:
    static std::string request(const std::string& address);
    static void clear();
    static int sequence;
    static std::vector<Order> bids;
    static std::vector<Order> asks;
    static int request_time_in_ms;
};