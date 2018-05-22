cmake_minimum_required(VERSION 3.9)
project(tcp_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(tcp_client main.cpp client.cpp client.h)
TARGET_LINK_LIBRARIES(tcp_client pthread)
