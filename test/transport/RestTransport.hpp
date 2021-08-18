#pragma once

#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

class RestTransport{
public:
    static std::string request(const std::string& address){

    return "";
    }

    std::vector<Order> bids;
    std::vector<Order> asks;
};