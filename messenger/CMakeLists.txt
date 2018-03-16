cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wshadow -Wfatal-errors")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0602")
endif()


set(CMAKE_CXX_STANDARD 17)

include(cmake/utils.cmake)
include(cmake/client.cmake)
include(cmake/server.cmake)