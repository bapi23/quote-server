#include "utils/FloatingP.hpp"
#include "Order.hpp"

bool operator==(const Order& lhs, const Order& rhs)
{
    return lhs.price == rhs.price &&
        lhs.size == rhs.size &&
        lhs.order_id == rhs.order_id;
}
