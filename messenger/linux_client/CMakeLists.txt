cmake_minimum_required(VERSION 2.8)
project(linux_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Client.cpp Client.h MessagesViewer.cpp MessagesViewer.h)
add_executable(linux_client ${SOURCE_FILES})