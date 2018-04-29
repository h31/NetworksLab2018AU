cmake_minimum_required(VERSION 2.8)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -Wextra")

set(SOURCE_FILES main.c "../commons/win.c")
add_executable(client_linux ${SOURCE_FILES})
if(WIN32)
	target_link_libraries(client_linux wsock32 ws2_32)
endif()	