cmake_minimum_required(VERSION 3.10)
project(qs_server)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(ext/json)
add_subdirectory(ext/curlpp)
add_subdirectory(protos)


find_package( Boost COMPONENTS system program_options REQUIRED )
find_package(OpenSSL REQUIRED)
find_package(cppzmq)

set(RAPIDJSON_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/ext/rapidjson/include)

set(SOURCE
    src/Order.cpp
    src/Market.cpp
    src/OrderBook.cpp
    src/product/ProductIdConnectionTable.cpp
    src/coinbase/CoinbaseFeedMessageHandler.cpp
    src/coinbase/CoinbaseFeedProduct.cpp
    src/coinbase/CoinbaseFeedClient.cpp
    src/coinbase/MessageToTrade.cpp
    transport/src/RestTransport.cpp
    transport/src/WebsocketTransport.cpp
    src/utils/uuid.cpp
)

add_executable(qs_server server.cpp ${SOURCE} src/ClientService.cpp)
target_include_directories(qs_server PRIVATE ext/websocketpp ext/curlpp/include transport/include include ${RAPIDJSON_INCLUDE_DIR})
target_link_libraries (qs_server ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp qs_protos)
set_property(TARGET qs_server APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_target_properties(qs_server PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(client client.cpp ${SOURCE})
target_include_directories(client PRIVATE ext/websocketpp ext/curlpp/include transport/include include ${RAPIDJSON_INCLUDE_DIR})
target_link_libraries (client ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp qs_protos)
set_property(TARGET client APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_target_properties(client PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_subdirectory(test)