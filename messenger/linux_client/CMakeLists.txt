cmake_minimum_required(VERSION 2.6)
project(client_linux)

SET(CMAKE_CXX_FLAGS "-std=c++0x")
add_executable(client_linux client.cpp)
target_link_libraries(client_linux pthread)
