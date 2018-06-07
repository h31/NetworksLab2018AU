cmake_minimum_required(VERSION 3.9)
project(server_windows)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
link_libraries(ws2_32 wsock32)

add_executable(server_windows main.c)