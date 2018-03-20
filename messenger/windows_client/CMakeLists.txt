cmake_minimum_required(VERSION 2.9)
project(client_windows)

link_libraries(ws2_32 wsock32)

set(SOURCE_FILES client.cpp)
add_executable(client_windows ${SOURCE_FILES})