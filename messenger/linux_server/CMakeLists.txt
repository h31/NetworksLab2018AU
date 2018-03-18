cmake_minimum_required(VERSION 2.8)
project(linux_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ../Messages.cpp ../Messages.h Server.cpp Server.h ClientHandler.cpp ClientHandler.h MessageSender.cpp MessageSender.h)
add_executable(linux_server ${SOURCE_FILES})