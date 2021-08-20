#pragma once

#include <string>
#include "utils/FloatingP.hpp"

struct Order{
    FloatingP price; 
    FloatingP size;
    std::string order_id;
};

bool operator==(const Order& lhs, const Order& rhs);
