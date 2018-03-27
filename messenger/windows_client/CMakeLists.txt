cmake_minimum_required(VERSION 2.9)
project(client_windows)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
link_libraries(ws2_32 wsock32)

add_executable(client_windows main.c)