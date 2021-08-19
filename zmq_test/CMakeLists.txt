cmake_minimum_required(VERSION 3.10)

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