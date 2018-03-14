cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wshadow -Wfatal-errors")
set(CMAKE_CXX_STANDARD 17)

include(cmake/utils.cmake)
include(cmake/client.cmake)
include(cmake/server.cmake)