#pragma once

#include "utils/FloatingP.hpp"

struct Order{
    FloatingP price; 
    FloatingP size;
    std::string order_id;
};

bool operator==(const Order& lhs, const Order& rhs)
{
    return lhs.price == rhs.price &&
        lhs.size == rhs.size &&
        lhs.order_id == rhs.order_id;
}
