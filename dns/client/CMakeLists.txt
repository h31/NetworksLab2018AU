cmake_minimum_required(VERSION 2.8)
project(client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Client.cpp Client.h main.cpp ../Message.cpp ../Message.h)
add_executable(client ${SOURCE_FILES})