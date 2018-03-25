cmake_minimum_required(VERSION 3.9)
project(server C)

set(CMAKE_C_STANDARD 11)

add_executable(server main.c utils.c)
include_directories("${CMAKE_HOME_DIRECTORY}/../common/")