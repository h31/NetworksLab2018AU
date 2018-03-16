cmake_minimum_required(VERSION 2.8)
project(server_linux)


set(SOURCE_FILES main.cpp ${COMMON_SOURCE_FILES})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#set_property(TARGET server_linux PROPERTY C_STANDARD 99)
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
endif()
