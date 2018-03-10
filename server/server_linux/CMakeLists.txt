cmake_minimum_required(VERSION 2.6)
project(server_linux C)

set(CMAKE_C_STANDARD 11)
add_executable(server_linux main.c utils.c lists.h cleaner.c handlers.c)
target_link_libraries(server_linux pthread)
include_directories("${CMAKE_HOME_DIRECTORY}/../../common/")