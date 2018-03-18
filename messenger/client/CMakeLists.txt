project(client)

set(HEADERS
        Client.h
        ClientSocket.h
        )

set(SOURCES
        Client.cpp
        ClientSocket.cpp
        main.cpp
        )

set(LIBRARIES
        socket
        )

IF (NOT WIN32)
    set(LIBRARIES ${LIBRARIES} pthread)
ENDIF()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
