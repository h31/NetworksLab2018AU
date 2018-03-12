cmake_minimum_required(VERSION 2.6)
project(client_linux C)

set(CMAKE_C_STANDARD 11)

add_executable(client_linux main.c utils.c handlers.c)
target_link_libraries(client_linux pthread)
include_directories("${CMAKE_HOME_DIRECTORY}/../../common/linux_common/")
