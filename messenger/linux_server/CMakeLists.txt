cmake_minimum_required(VERSION 3.9)
project(linux_server)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(linux_server
        linux_server.c)

target_link_libraries(linux_server pthread)