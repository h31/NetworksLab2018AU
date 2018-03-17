project(linux_server)

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
        pthread
        )

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})