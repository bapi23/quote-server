cmake_minimum_required(VERSION 3.10)

find_package(Protobuf REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
        qs_protos/Message.proto)


add_library(qs_protos ${PROTO_SRCS})

get_filename_component(PROTS_INCLUDE_DIR ${PROTO_HDRS} DIRECTORY)
message("PROTOS DIR: ${PROTS_INCLUDE_DIR} ")

target_include_directories(qs_protos PUBLIC ${PROTS_INCLUDE_DIR})
target_link_libraries(qs_protos ${PROTOBUF_LIBRARIES} ${Protobuf_LITE_LIBRARIES})
