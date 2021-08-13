cmake_minimum_required(VERSION 3.16)
project(qos_server)


set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(ext/json)
add_subdirectory(ext/curlpp)

find_package( Boost COMPONENTS system REQUIRED )
find_package(OpenSSL REQUIRED)
find_package(cppzmq)

add_executable(qos_server main.cpp)

target_include_directories(qos_server PRIVATE ext/websocketpp ext/curlpp/include include)
target_link_libraries (qos_server ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp)
set_property(TARGET qos_server APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_target_properties(qos_server PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

add_executable(test_qos_server_component test/test_qos_server_component.cpp ext/catch2/catch2/catch.cpp)
target_include_directories(test_qos_server_component PRIVATE ext/websocketpp ext/curlpp/include ext/catch2/ include)
target_link_libraries (test_qos_server_component ${Boost_LIBRARIES} cppzmq pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp)
set_property(TARGET test_qos_server_component APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set_target_properties(test_qos_server_component PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

