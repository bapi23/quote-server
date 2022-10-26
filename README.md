# Simple Quote Server implementation

QS Server can be used mainly to obtain data from the Coinbase pro api. It might be also extended with other APIs
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
/app/src/qs_server/build/client --prod-ids sub ETH-USD 1 sub BTC-USD 2 --server-endpoint tcp://127.0.0.1:10090
```
Will subscribe for ETH-USD after 1 second from running client and BTC-USD after 2 seconds.
You can also write subscribe/unsubscribe scenarios:
```
/app/src/qs_server/build/client --prod-ids sub ETH-USD 1 unsub ETH-USD 10 --server-endpoint tcp://127.0.0.1:10090
```

### Run it natively
You can also run it natively by running
```
qs-server
```
And run in another terminal
```
client --prod-ids sub ETH-USD 1 unsub ETH-USD 10 --server-endpoint tcp://127.0.0.1:10090
```

## Implementation documentation
Implementation documentation and message definitions + workflow can be found inside doc folder.

##### TODO list
- implement dynamic endpoint allocation with the usage of https://api.pro.coinbase.com/products/
- Implement subscription confirmation between FeedClient and ClientRegister
- floating point precision


#### Things to discuss:
1. Scaling:

- Current Implementation should be splitted. ClientService shouild be a load balancing separated service and provide endpoints which are best fit for the client based on network latency, servers occupation, existing connections for product ids etc.

- There should be another service which is capable of starting new server nodes (also for the same product ids) if all other are occpied heavily. Client Service should be able to request a new node for the client or bypass it to another node if current one is unstable/have low performance.

2. Latency

- Tests can be used to measure how system will behave under high load.
- Metrics should be used to determine messages frequency and delays between components (based on time stamps)

Publisher profiling:
Performance of serializing 100k of Orders 
with protocol buffers:
```
Publishing (creating proto) took 8567 [µs]
Publishing (dumping proto) took 11654 [µs]
Publishing (sending proto) took 17 [µs]
```
with rapid json
```
Publishing (creating json) took 64794 [µs]
Publishing (dumping json) took 93244 [µs]
Publishing (sending json) took 19 [µs]
```

- Ideas on how to see delays:
  1. Add metrics on how long messages are waiting in the queue
  2. Estimate average times for product id handling
