#include "FeedClient.hpp"
#include <unordered_map>

class FeedClientSpy: public FeedClient
{
public:
    virtual void subscribe(const std::string& product_id, std::weak_ptr<ProductChangeListener> listener){
        m_listeners[product_id] = listener;
    }
    virtual void unsubscribe(const std::string& product_id){
        m_listeners.erase(product_id);
    }

std::unordered_map<std::string, std::weak_ptr<ProductChangeListener>> m_listeners;
};