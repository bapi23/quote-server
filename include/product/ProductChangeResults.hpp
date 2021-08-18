#pragma once

#include <vector>
#include "Order"

class ProductChangeResults{
    std::vector<Order> add;
    std::vector<Order> remove;
    std::vector<Order> update;
};