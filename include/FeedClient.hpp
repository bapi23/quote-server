#include <string>

#include <ProductChangeListener.hpp>

class FeedClient{
public:
    void subscribe(const std::string& product_id, ProductChangeListener* listener);
    void unsubscribe(const std::string& product_id);
    virtual ~FeedClient(){};
};