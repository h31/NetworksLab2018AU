cmake_minimum_required(VERSION 3.5)
project(server)

set(CMAKE_CXX_STANDARD 11)

add_executable(server main.cpp message.h)
