cmake_minimum_required(VERSION 3.10)
project(dns)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp common.cpp common.h)
add_executable(client client.cpp common.cpp common.h)