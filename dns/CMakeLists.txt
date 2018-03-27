cmake_minimum_required(VERSION 2.9)
project(dns)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(DNS_CLIENT client_main.c dns_header.h)
add_executable(dns_client ${DNS_CLIENT} dns_header.c)

set(DNS_SERVER server_main.c dns_header.h)
add_executable(dns_server ${DNS_SERVER} dns_header.c)

