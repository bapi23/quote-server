#pragma once

#include "utils/FloatingP.hpp"

struct Order{
    FloatingP price; 
    FloatingP size;
    std::string order_id;
};
