cmake_minimum_required(VERSION 2.8)
project(dns)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SERVER_SRC "server/main.c")
set(CLIENT_SRC "client/main.c")

add_executable(server.out ${SERVER_SRC})
add_executable(client.out ${CLIENT_SRC})