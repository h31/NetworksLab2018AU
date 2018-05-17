cmake_minimum_required(VERSION 3.9)
project(audns_common C)

add_definitions(-DLOG_ENABLE)
add_library(${PROJECT_NAME}
        byte_buf.c
        dns_proto.c
        dns_socket_utils.c
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
