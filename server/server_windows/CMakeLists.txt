cmake_minimum_required(VERSION 2.6)
project(server_windows C)

set(CMAKE_C_STANDARD 11)
add_executable(server_windows main.c utils.c lists.h cleaner.c handlers.c)
include_directories("${CMAKE_HOME_DIRECTORY}/../../common/windows_common/")