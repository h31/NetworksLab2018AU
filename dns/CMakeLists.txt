cmake_minimum_required(VERSION 2.8)
project(dns)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

set(COMMON_SRC "common/connection.c" "common/handling.c")
set(SERVER_SRC "server/main.c" "${COMMON_SRC}")
set(CLIENT_SRC "client/main.c" "client/request.c" "${COMMON_SRC}")

add_executable(server.out ${SERVER_SRC})
add_executable(client.out ${CLIENT_SRC})

