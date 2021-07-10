cmake_minimum_required(VERSION 3.16)
project(qos_server)


set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(ext/json)
add_subdirectory(ext/curlpp)

find_package( Boost COMPONENTS system REQUIRED )
find_package(OpenSSL REQUIRED)

add_executable(qos_server main.cpp)

target_include_directories(qos_server PRIVATE ext/websocketpp ext/curlpp/include)

target_link_libraries (qos_server ${Boost_LIBRARIES} pthread ${OPENSSL_LIBRARIES} nlohmann_json::nlohmann_json curlpp)
set_property(TARGET qos_server APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

