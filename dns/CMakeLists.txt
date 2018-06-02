cmake_minimum_required(VERSION 2.8)
project(dns)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

set(SERVER_SRC "server/server.cpp" )
set(CLIENT_SRC "client/client.cpp" )

add_executable(server.out ${SERVER_SRC})
add_executable(client.out ${CLIENT_SRC})

