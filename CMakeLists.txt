cmake_minimum_required(VERSION 3.10)
project(qos_server)


set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(ext/json)
add_subdirectory(ext/curlpp)

find_package( Boost COMPONENTS system program_options REQUIRED )
find_package(OpenSSL REQUIRED)
find_package(cppzmq)

add_executable(qos_server server.cpp)
target_include_directories(qos_server PRIVATE ext/websocketpp ext/curlpp/include include)
target_link_libraries (qos_server ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp)
set_property(TARGET qos_server APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_target_properties(qos_server PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(client client.cpp)
target_include_directories(client PRIVATE ext/websocketpp ext/curlpp/include include)
target_link_libraries (client ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp)
set_property(TARGET client APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_target_properties(client PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(MarketComponentTest test/MarketComponentTest.cpp ext/catch2/catch2/catch.cpp)
target_include_directories(MarketComponentTest PRIVATE ext/websocketpp ext/curlpp/include ext/catch2/ include)
target_link_libraries (MarketComponentTest ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp)
set_property(TARGET MarketComponentTest APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set_target_properties(MarketComponentTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )



add_executable(zmq_test_pub zmq_test_pub.cpp)
target_include_directories(zmq_test_pub PRIVATE include)
target_link_libraries (zmq_test_pub cppzmq pthread ${OPENSSL_LIBRARIES} )

set_target_properties(zmq_test_pub PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(zmq_test_sub zmq_test_sub.cpp)
target_include_directories(zmq_test_sub PRIVATE include)
target_link_libraries (zmq_test_sub cppzmq pthread ${OPENSSL_LIBRARIES} )

set_target_properties(zmq_test_sub PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )