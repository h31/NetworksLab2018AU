cmake_minimum_required(VERSION 3.9)
project(windows_client)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(windows_client
        windows_client.c)

target_link_libraries(windows_client pthread wsock32)