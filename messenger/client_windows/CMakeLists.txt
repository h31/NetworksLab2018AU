cmake_minimum_required(VERSION 2.9)
project(client_windows)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(client_windows ${SOURCE_FILES})
