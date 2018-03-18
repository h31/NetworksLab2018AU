cmake_minimum_required(VERSION 3.9)
project(elegram)

set(CMAKE_C_STANDARD 11)

if(WIN32)
    target_link_libraries(MyExe wsock32 ws2_32)
endif()

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
