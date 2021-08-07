#include "ProductChangeListener.hpp"
#include "ProductChange.hpp"
#include "MessageReceiver.hpp"

class CoinbaseFeedMessageHandler: public MessageReceiver{
public:
    CoinbaseFeedMessageHandler(ProductChangeListener* listener):m_listener(listener){

    }
    void onMessageReceved(const std::string& msg){
        //.. create change
        ProductChange* pc;
        m_listener->onProductChange(pc);
    }

private:
    ProductChangeListener* m_listener;
};