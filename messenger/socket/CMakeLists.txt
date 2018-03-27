project(socket)

set(HEADERS
        Socket.h
        Message.h
        )

set(SOURCES
        Socket.cpp
        Message.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ./)