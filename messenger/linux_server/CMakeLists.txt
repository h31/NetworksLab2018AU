cmake_minimum_required(VERSION 3.9)
project(server_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(server_linux main.c)
target_link_libraries(server_linux pthread)