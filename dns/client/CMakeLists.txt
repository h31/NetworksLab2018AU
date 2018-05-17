cmake_minimum_required(VERSION 3.9)
project(audns_client C)

add_executable(${PROJECT_NAME} main.c dns_client.c)
target_link_libraries(${PROJECT_NAME} audns_common)

target_include_directories(${PROJECT_NAME} PRIVATE ../common/include)
target_include_directories(${PROJECT_NAME} PRIVATE include)
