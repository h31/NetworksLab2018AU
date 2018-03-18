cmake_minimum_required(VERSION 2.8)
project(server_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -Wextra")

set(SOURCE_FILES main.c)
add_executable(server_linux ${SOURCE_FILES})
if(WIN32)
	target_link_libraries(server_linux wsock32 ws2_32)
endif()	