cmake_minimum_required(VERSION 2.8)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SERVER_SRC "server/main.c" "${COMMON_SRC}")
set(CLIENT_SRC "client/main.c" "${COMMON_SRC}")

add_executable(server.out ${SERVER_SRC})
add_executable(client.out ${CLIENT_SRC})