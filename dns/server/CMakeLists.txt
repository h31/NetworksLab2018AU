cmake_minimum_required(VERSION 3.1)
project(server)

set(CMAKE_CXX_STANDARD 11)

set(HEADERS
        Server.h
        ../common/dnsUtils.h
        )

set(SOURCES
        Server.cpp
        ../common/dnsUtils.cpp
        main.cpp
        )

set(LIBRARIES
        pthread
        dnsUtils
        )

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})