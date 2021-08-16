#QOS Server implementation

QOS Server can be used mainly to obtain data from the Coinbase pro api. It might be also extended with other APIs
The main purpose of the project is to obtain feed data in fast and reliable way.


## How to build it

### Dokerize it
Whole project is dockerized so you can build it using Dockerfile:
```
docker build -t qs_server .
```
All dependecies are contained inside Dockerfile so setup your local environement accordingly to it.

### Build it natively
Look for the dependencies inside the Dockerfile
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE ..
make -j 4
```

It will build qs_server, example of the client and tests

## How to run it

### Use docker compose
You can just run 
```
docker-compose up
```
It will run demo with the server and the client configured for ETH-USD and BTC-USD product ids

Client is implemented in a way to execute provided scenario. For example:
```
/app/src/qs_server/build/client --prod-ids sub ETH-USD 1 sub BTC-USD 2 --server-endpoint tcp://127.0.0.1:9090
```
Will subscribe for ETH-USD after 1 second from running client and BTC-USD after 2 seconds.
You can also write subscribe/unsubscribe scenarios:
```
/app/src/qs_server/build/client --prod-ids sub ETH-USD 1 unsub ETH-USD 10 --server-endpoint tcp://127.0.0.1:9090
```

### Run it natively
You can also run it natively by running
```
qs-server
```
And run in another terminal
```
client --prod-ids sub ETH-USD 1 unsub ETH-USD 10 --server-endpoint tcp://127.0.0.1:9090
```

## Implementation documentation
Implementation documentation and message definitions + workflow can be found inside doc folder.

#
#
##### TODO list
- Change message received format inside CoinbaseFeedListener to json
- Add sources to speed up compilation
- Implement subscription confirmation between FeedClient and ClientRegister
- Merge full order with feed messages to get better performance here: include/coinbase/CoinbaseFeedListener.hpp:50