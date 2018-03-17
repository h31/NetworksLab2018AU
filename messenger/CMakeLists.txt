cmake_minimum_required(VERSION 3.5)
project(messenger)

set(CMAKE_CXX_STANDARD 11)

set(COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${COMMON_SOURCE_DIR})

file(GLOB COMMON_SOURCE_FILES
        ${COMMON_SOURCE_DIR}/*.cpp
        ${COMMON_SOURCE_DIR}/*.h)

if(UNIX)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O0 -ggdb")
set(SERVER_PROJECT "linux_server")
set(CLIENT_PROJECT "linux_client")
else()
set(SERVER_PROJECT "windows_server")
set(CLIENT_PROJECT "windows_client")
endif()

add_subdirectory(${SERVER_PROJECT})
add_subdirectory(${CLIENT_PROJECT})

