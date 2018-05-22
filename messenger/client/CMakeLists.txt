cmake_minimum_required(VERSION 3.9)
project(elegram_client C)

find_package(Threads REQUIRED)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} elegram_common Threads::Threads)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ../common/include)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE .)
include_directories(${PROJECT_NAME} ${CURSES_INCLUDE_DIR})
