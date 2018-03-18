cmake_minimum_required(VERSION 3.9)
project(windows_server)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(windows_server main.cpp)
target_link_libraries(windows_server wsock32 ws2_32)