cmake_minimum_required(VERSION 2.9)
project(dns)

set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wall -Wextra")

add_executable(server server.cpp)

