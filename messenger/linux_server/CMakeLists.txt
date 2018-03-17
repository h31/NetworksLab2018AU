cmake_minimum_required(VERSION 2.6)
project(server_linux)

SET(CMAKE_CXX_FLAGS "-std=c++0x")
add_executable(server_linux server.cpp)
target_link_libraries(server_linux pthread)
