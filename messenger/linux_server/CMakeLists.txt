cmake_minimum_required(VERSION 2.8)
project(server_linux)

find_package(Threads REQUIRED)

set(SOURCE_FILES main.cpp ${COMMON_SOURCE_FILES})
add_executable(server_linux ${SOURCE_FILES})

set_property(TARGET server_linux PROPERTY C_STANDARD 99)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(server_linux PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(server_linux "${CMAKE_THREAD_LIBS_INIT}")
endif()
