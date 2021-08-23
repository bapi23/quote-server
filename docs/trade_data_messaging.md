#### QS server ouptut data
Client server can provide the following data:
OrderBook message with up-to-date asks and bids orders contining size, price and order id.
```json
{
"bids":{
        {
            "price": 34.4,
            "size": 40.5,
            "order_id": "3ef1f28d-af48-4a04-be65-3f067071d31b"
        },
        {
            "price": 33.4,
            "size": 40.5,
            "order_id": "3ef1f28d-af48-4a04-be65-3f067071d31b"
        },
        ...
},
"asks":{
        {
            "price": 31.4,
            "size": 45.5,
            "order_id": "3ef1f28d-af48-4a04-be65-3f067071d31b"
        },
        {
            "price": 30.4,
            "size": 23.5,
            "order_id": "3ef1f28d-af48-4a04-be65-3f067071d31b"
        },
        ...
}
}
```



###Trade messages
QS server provides also trade messages with additional information.
```json
{
    "type":"match"
    "product_id":"ETH-USD"
    "order_id":"d50ec984-77a8-460a-b958-66f114b0de9b"
    "remaining_size": 45.6
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