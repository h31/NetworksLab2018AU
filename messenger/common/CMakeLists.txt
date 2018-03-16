cmake_minimum_required(VERSION 3.9)
project(elegram_common C)

add_library(${PROJECT_NAME} message_format.c string_utils.c)

target_include_directories(${PROJECT_NAME} PRIVATE include)
