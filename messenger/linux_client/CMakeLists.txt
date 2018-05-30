cmake_minimum_required(VERSION 2.9)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(client_linux main.c)
target_link_libraries(client_linux pthread)