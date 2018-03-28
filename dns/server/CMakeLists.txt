cmake_minimum_required(VERSION 2.8)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SOURCE_FILES main.c ../commons/dns.c)
add_executable(dns_server ${SOURCE_FILES})