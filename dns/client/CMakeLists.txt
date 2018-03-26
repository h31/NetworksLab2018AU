cmake_minimum_required(VERSION 2.8)
project(dns_client)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
include_directories(../)

set(SOURCE_FILES main.c)
add_executable(dns_client ${SOURCE_FILES})
