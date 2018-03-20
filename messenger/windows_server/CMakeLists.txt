cmake_minimum_required(VERSION 2.9)
project(server_windows)

link_libraries(ws2_32 wsock32)

set(SOURCE_FILES server.cpp)
add_executable(server_windows ${SOURCE_FILES})