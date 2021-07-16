#include "OrderBook.hpp"
#include "Trade.hpp"

class Client{
public:
    Client(std::string clientId);
    void onOrderBook(const OrderBook& orderBook){};
    void onTrade(const Trade& trade){};

private:
    std::string client_id;
};