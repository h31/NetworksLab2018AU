cmake_minimum_required(VERSION 2.9)
project(client_linux)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES client.cpp)
add_executable(client_linux ${SOURCE_FILES})
