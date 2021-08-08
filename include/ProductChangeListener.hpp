#pragma once

#include <memory>

#include "ProductChange.hpp"

class OrderBook;

class ProductChangeListener{
public:
    virtual void onProductChange(std::unique_ptr<ProductChange> pc) = 0;
    virtual ~ProductChangeListener(){};
};