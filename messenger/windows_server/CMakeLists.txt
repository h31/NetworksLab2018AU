cmake_minimum_required(VERSION 3.9)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(server_linux main.c)