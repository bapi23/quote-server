## QOS Server design

### Transport side

##### FeedClient sequence diagram
WebsocketTransport signals message reception by MessageReceiver interface
```plantuml
@startuml "Message Received"
CoinbaseFeedApi->WebsocketTransport: onMessage(msg)
WebsocketTransport->CoinbaseFeedClient : onMessageReceived(msg)
CoinbaseFeedClient->CoinbaseFeedListener : onMessageReceived(msg)
CoinbaseFeedListener->CoinbaseFeedMessageHandler: OnMessageReceiver(msg)
CoinbaseFeedMessageHandler->ProductChangeListener: OnProductChanged(ProductChange)
@enduml
```

##### FeedClient class diagram
CoinbaseFeedClient, CoinbaseFeedListener and CoinbaseMessageHandler implements some kind of decorator pattern by implementing MessageReceiver interface.

CoinbaseFeedClient implements FeedClient interface and contains all 
CoinbaseFeedListeners. On message reception it calls onMessageReceived() only on CoinbaseFeedListener which listens for received productId.

CoinbaseFeedListener listens only for one productId and implements logic specific to coinbase api - requesting all orderbook entries at the beggining, queuing messages and ensuring the order of messages by checking sequence number.

CoinbaseFeedMessageHandler receives coinbase message for specific productId and creates ProductChange from it. Then it notifies Product though ProductChangeListener interface.

Any market api which provides feed messaging might implement FeedClient interface in a similar way and use some part of the existing implementation if possible.

```plantuml
@startuml "FeedClient class diagram"
abstract FeedClient
class CoinbaseFeedClient

class CoinbaseFeedListener
class CoinbaseFeedMessageHandler
abstract MessageReceiver
abstract ProductListener

CoinbaseFeedClient "1" *-- "many" CoinbaseFeedListener : contains
FeedClient <|-- CoinbaseFeedClient
MessageReceiver <|-- CoinbaseFeedClient
MessageReceiver <|-- CoinbaseFeedMessageHandler
MessageReceiver <|-- CoinbaseFeedListener

CoinbaseFeedListener "1" *-- "1" CoinbaseFeedMessageHandler : contains
CoinbaseFeedListener "1" *-- "1" ProductListener : contains
@enduml
```


##### Top level Market class diagram
Simple top level class diagram which shows compositions of the main domain parts
```plantuml
@startuml "Market class diagram"
abstract FeedClient
class Market

class Product
class Client
abstract FeedClient
abstract OrderBook
abstract ProductChangeListener

Market "1" o-- "*" Client
Product "1" o-- "*" Client
Market "1" o-- "1" FeedClient
Product "1" o-- "1" OrderBook
ProductChangeListener <|-- Product
@enduml
```

##### Client registration
Client needs to get clientId to be able to subscribe and receive Orderbook and Trade messages
```plantuml
@startuml "Client registration"
ClientTransport->ClientServer: register()
ClientServer->ClientTransport: sendClientId(id)
ClientTransport->ClientServer: subscribe(clientId, productId)
@enduml
@enduml
```

##### Sequence diagram after creating ProductChange:
Classes wchich implements ProductChange interface encapsulates logic related to specifig changes. New ProductChange might be added and FeedClient interface implementation can create it. In Coinbase use case CoinbaseFeedMessageHandler class creates concrete ProductChange's

```plantuml
@startuml "ProductChangeReceived"
Product->ProductChange : updateOrderBook(OrderBook): (updated) bool
Product->ProductChange : getTrade(msg): Trade
Product->Client: if(updated): onOrderBook(OrderBookView)
Client->ClientListener: notifyOrderBookChanged(clientId, OrderBookView):
ClientListener->ClientServer: sendOrderBookUpdate(json)
Product->Client: if(trade) OnTrade(Trade)
Client->ClientListener: notifyOrderBookChanged(clientId,orderBookView->toJSON()):
ClientListener->ClientServer: sendOrderBookUpdate(json)
@enduml
```

##### ProductData subclasses
```plantuml
@startuml "ProductChange class"
abstract ProductChange
class ProductChangeDone
class ProductChangeOpen
class ProductChangeMatch
class ProductChangeChanged
ProductChange <|-- ProductChangeDone
ProductChange <|-- ProductChangeOpen
ProductChange <|-- ProductChangeMatch
ProductChange <|-- ProductChangeChanged
@enduml
```

##### ClientServer ouptut data
Client server can provide the following data:
OrderBook message with currently avaialable capacity and price
```json
{
"bid":{
        {
            "price": 34.4,
            "size": 40.5
        },
        {
            "price": 33.4,
            "size": 40.5
        },
        ...
},
"ask":{
        {
            "price": 31.4,
            "size": 45.5
        },
        {
            "price": 30.4,
            "size": 23.5
        },
        ...
}
}
```

Trade messages
```json
{
    "type":"match"
    "product_id":"ETH-USD"
    "order_id":"d50ec984-77a8-460a-b958-66f114b0de9b"
}
```
```json
{
    "type":"done"
    "product_id":"ETH-USD"
    "order_id":"d50ec984-77a8-460a-b958-66f114b0de9b"
}
```
```json
{
    "type":"open"
    "product_id":"ETH-USD"
    "order_id":"d50ec984-77a8-460a-b958-66f114b0de9b"
    "remaining_size": 30.5
}
```
```json
{
    "type":"change"
    "product_id":"ETH-USD"
    "order_id":"d50ec984-77a8-460a-b958-66f114b0de9b"
    "remaining_size": 30.5
}
```