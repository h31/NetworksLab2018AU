cmake_minimum_required(VERSION 3.9)
project(windows_server)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_STANDARD 99)

add_executable(windows_server
        windows_server.c)

target_link_libraries(windows_server pthread wsock32)