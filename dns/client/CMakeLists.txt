cmake_minimum_required(VERSION 3.5)
project(client)

set(CMAKE_CXX_STANDARD 11)

add_executable(client main.cpp message.h utils.h)
