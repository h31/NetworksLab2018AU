cmake_minimum_required(VERSION 2.8)
project(server)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

set(COMMON_SRC "common/communication.c" "common/network.c" "common/threads.c")
set(SERVER_SRC "server/clients.c" "server/main.c" "${COMMON_SRC}")
set(CLIENT_SRC "client/main.c" "${COMMON_SRC}")

add_executable(server.out ${SERVER_SRC})
add_executable(client.out ${CLIENT_SRC})

if (WIN32)
    target_link_libraries(server.out wsock32)
    target_link_libraries(client.out wsock32)
endif()
