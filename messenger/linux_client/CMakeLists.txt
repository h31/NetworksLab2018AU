cmake_minimum_required(VERSION 3.9)
project(linux_client)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(linux_client
        linux_client.c)

target_link_libraries(linux_client pthread)