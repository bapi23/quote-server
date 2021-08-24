
cmake_minimum_required(VERSION 3.10)


set(RAPIDJSON_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/ext/rapidjson/include)

set(SOURCE
    ${PROJECT_SOURCE_DIR}/src/coinbase/CoinbaseFeedMessageHandler.cpp
    ${PROJECT_SOURCE_DIR}/src/Order.cpp
    ${PROJECT_SOURCE_DIR}/src/Market.cpp
    ${PROJECT_SOURCE_DIR}/src/OrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/product/ProductIdConnectionTable.cpp
    ${PROJECT_SOURCE_DIR}/src/coinbase/CoinbaseFeedProduct.cpp
    ${PROJECT_SOURCE_DIR}/src/coinbase/CoinbaseFeedClient.cpp

    ${PROJECT_SOURCE_DIR}/src/utils/uuid.cpp
)

set(TEST_TRANSPORT_SOURCE
    ${PROJECT_SOURCE_DIR}/test/transport/src/RestTransport.cpp
    ${PROJECT_SOURCE_DIR}/test/transport/src/WebsocketTransport.cpp
)

set(REAL_TRANSPORT_SOURCE
    ${PROJECT_SOURCE_DIR}/transport/src/RestTransport.cpp
    ${PROJECT_SOURCE_DIR}/transport/src/WebsocketTransport.cpp
)

add_executable(MarketComponentTest MarketComponentTest.cpp 
                    ${PROJECT_SOURCE_DIR}/ext/catch2/catch2/catch.cpp 
                    ${SOURCE}
                    ${TEST_TRANSPORT_SOURCE})

target_include_directories(MarketComponentTest 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/ext/websocketpp 
                                ${PROJECT_SOURCE_DIR}/ext/curlpp/include 
                                ${PROJECT_SOURCE_DIR}/ext/catch2/  
                                ${PROJECT_SOURCE_DIR}/include
                                transport/include)

target_link_libraries (MarketComponentTest 
                            ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} 
                            nlohmann_json::nlohmann_json curlpp qs_protos)
                            
set_property(TARGET MarketComponentTest APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set_target_properties(MarketComponentTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(CoibaseFeedTest CoinbaseFeedTest.cpp 
                                ${PROJECT_SOURCE_DIR}/ext/catch2/catch2/catch.cpp
                                ${SOURCE}
                                ${TEST_TRANSPORT_SOURCE})
                                
target_include_directories(CoibaseFeedTest 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/ext/websocketpp 
                                ${PROJECT_SOURCE_DIR}/ext/curlpp/include 
                                ${PROJECT_SOURCE_DIR}/ext/catch2/ 
                                ${PROJECT_SOURCE_DIR}/include
                                transport/include)

target_link_libraries (CoibaseFeedTest ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp qs_protos)
set_property(TARGET CoibaseFeedTest APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set_target_properties(CoibaseFeedTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )


add_executable(RealCoinbaseFeedTest RealCoinbaseFeedTest.cpp 
                                ${PROJECT_SOURCE_DIR}/ext/catch2/catch2/catch.cpp
                                ${PROJECT_SOURCE_DIR}/src/
                                ${SOURCE}
                                ${REAL_TRANSPORT_SOURCE})
                                
target_include_directories(RealCoinbaseFeedTest 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/ext/websocketpp 
                                ${PROJECT_SOURCE_DIR}/ext/curlpp/include 
                                ${PROJECT_SOURCE_DIR}/ext/catch2/ 
                                ${PROJECT_SOURCE_DIR}/include
                                ${PROJECT_SOURCE_DIR}/transport/include)

target_link_libraries (RealCoinbaseFeedTest ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp qs_protos)
set_property(TARGET RealCoinbaseFeedTest APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set_target_properties(RealCoinbaseFeedTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(PublisherPerformanceTest PublisherPerformanceTest.cpp 
                    ${PROJECT_SOURCE_DIR}/ext/catch2/catch2/catch.cpp
                    ${PROJECT_SOURCE_DIR}/src/
                    ${SOURCE}
                    ${TEST_TRANSPORT_SOURCE})

target_include_directories(PublisherPerformanceTest 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/ext/websocketpp 
                                ${PROJECT_SOURCE_DIR}/ext/curlpp/include 
                                ${PROJECT_SOURCE_DIR}/ext/catch2/ 
                                ${PROJECT_SOURCE_DIR}/include
                                transport/include
                                ${RAPIDJSON_INCLUDE_DIR})

target_link_libraries (PublisherPerformanceTest ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp qs_protos)
set_property(TARGET RealCoinbaseFeedTest APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set_target_properties(PublisherPerformanceTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )