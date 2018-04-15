cmake_minimum_required(VERSION 3.9)
project(audns_server C)

add_executable(${PROJECT_NAME} main.c dns_server.c)
target_link_libraries(${PROJECT_NAME} audns_common)
add_definitions(-DLOG_ENABLE)

target_include_directories(${PROJECT_NAME} PRIVATE ../common/include)
target_include_directories(${PROJECT_NAME} PRIVATE include)
