cmake_minimum_required(VERSION 2.6)
project(client C)

set(CMAKE_C_STANDARD 11)

add_executable(client main.c utils.c)
include_directories("${CMAKE_HOME_DIRECTORY}/../common/")