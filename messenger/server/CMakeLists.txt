project(server)

set(HEADERS
        Server.h
        ServerSocket.h
        )

set(SOURCES
        main.cpp
        Server.cpp
        ServerSocket.cpp
        )

set(LIBRARIES
        socket
        )

IF (NOT WIN32)
    set(LIBRARIES ${LIBRARIES} pthread)
ENDIF()

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})