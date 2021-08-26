#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <atomic>

#include "utils/uuid.hpp"
#include "CoinbaseEndpointService.hpp"
#include "product/ProductSubscriber.hpp"

class ClientService{
public:
    ClientService(ProductSubscriber* productSubscriber, const std::string& endpoint_address);
    void run();

private:
    void runImpl();

    std::atomic<bool> isRunning{true};
    std::unordered_set<std::string> registeredClientIds;
    ProductSubscriber* m_productSubscriber;
    std::string m_endpoint_address;
};