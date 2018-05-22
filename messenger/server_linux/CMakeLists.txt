cmake_minimum_required(VERSION 2.9)
project(server_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SOURCE_FILES main.c)
add_executable(server_linux ${SOURCE_FILES})
target_link_libraries(server_linux pthread)
