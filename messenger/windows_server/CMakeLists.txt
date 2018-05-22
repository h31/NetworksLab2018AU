cmake_minimum_required(VERSION 3.9)
project(tcp)

set(CMAKE_CXX_STANDARD 11)

add_executable(tcp main.cpp vector.h server.cpp server.h message.cpp message.h)
TARGET_LINK_LIBRARIES(tcp pthread)
