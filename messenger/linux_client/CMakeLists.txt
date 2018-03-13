cmake_minimum_required(VERSION 2.8)
project(client_linux)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(SOURCE_FILES main.cpp ${COMMON_SOURCE_FILES})
add_executable(client_linux ${SOURCE_FILES})
set_property(TARGET client_linux PROPERTY C_STANDARD 99)
