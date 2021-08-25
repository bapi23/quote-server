#### QS server ouptut data
Client server can provide the following data:
OrderBook message with up-to-date asks and bids orders contining size, price and order id.
All messages definition are located under protos/qs_protos/Message.proto



###Trade messages
QS server provides also trade messages with additional information. Currentyl supported trade types:
```c++
enum TradeType {
    TradeType_ACTIVATE = 0;
    TradeType_CHANGE = 1;
    TradeType_DONE = 2;
    TradeType_MATCH = 3;
    TradeType_OPEN = 4;
}
```
All details can be found in the protos/qs_protos/Message.proto file