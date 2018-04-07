cmake_minimum_required(VERSION 2.8)
project(server_linux)

set(SOURCE_FILES main.cpp ${COMMON_SOURCE_FILES})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
