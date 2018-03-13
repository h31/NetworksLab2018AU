cmake_minimum_required(VERSION 2.8)
project(server_linux)

set(SOURCE_FILES main.cpp ${COMMON_SOURCE_FILES})
add_executable(server_linux ${SOURCE_FILES})
set_property(TARGET server_linux PROPERTY C_STANDARD 99)
