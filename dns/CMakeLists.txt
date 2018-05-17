cmake_minimum_required(VERSION 3.9)
project(audns)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
