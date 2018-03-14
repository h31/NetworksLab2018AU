cmake_minimum_required(VERSION 2.9)
project(server_windows)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
link_libraries(ws2_32 wsock32)

set(SOURCE_FILES main.c)
add_executable(server_windows ${SOURCE_FILES})
