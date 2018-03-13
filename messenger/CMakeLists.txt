cmake_minimum_required(VERSION 3.5)
project(messenger)

set(CMAKE_CXX_STANDARD 11)

set(COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${COMMON_SOURCE_DIR})

file(GLOB COMMON_SOURCE_FILES
        ${COMMON_SOURCE_DIR}/*.cpp
        ${COMMON_SOURCE_DIR}/*.h)

#set(COMMON_SOURCE_FILES
#        src/ElegramAll.h src/ElegramFwd.h
#        )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O0 -ggdb")

#set(CMAKE_BUILD_TYPE DEBUG)
add_subdirectory(linux_server)
add_subdirectory(linux_client)

