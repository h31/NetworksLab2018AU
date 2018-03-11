cmake_minimum_required(VERSION 2.9)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SOURCE_FILES main.c)
add_executable(client_linux ${SOURCE_FILES})