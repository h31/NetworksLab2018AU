cmake_minimum_required(VERSION 2.9)
project(server_linux)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -fpermissive")

set(SOURCE_FILES server.cpp)
add_executable(server_linux ${SOURCE_FILES})
