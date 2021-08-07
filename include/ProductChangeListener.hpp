#include <memory>

#include "ProductChange.hpp"

class OrderBook;

class ProductChangeListener{
public:
    virtual void onProductChange(ProductChange* pc) = 0;
    virtual ~ProductChangeListener(){};
};