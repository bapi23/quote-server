#pragma once
#include <vector>

#include "Order.hpp"
#include "product/ProductChangeDone.hpp"
#include "product/ProductChangeOpen.hpp"

std::vector<Order> generateData(unsigned firstOrderId, int numberOfSamples){
    std::vector<Order> orders;
    for(int i = 0; i <= 1000; ++i){
        orders.push_back(Order{3.0, 6.0, std::to_string(firstOrderId+i)});
    }
    return orders;
}

std::vector<std::unique_ptr<ProductChange>> generateProductChanges(unsigned startId, const std::string& productId, unsigned numberOfChanges){
    std::vector<std::unique_ptr<ProductChange>> changes;
    for(int i = 0; i <= numberOfChanges; i+=2){
        auto done = std::make_unique<ProductChangeDone>(std::to_string(startId), Side::Buy, productId, startId + i);
        auto open = std::make_unique<ProductChangeOpen>(std::to_string(startId), 3.0, 4.6, Side::Buy, productId, startId + i);
        changes.push_back(std::move(done));
        changes.push_back(std::move(open));
    }
    return changes;
}