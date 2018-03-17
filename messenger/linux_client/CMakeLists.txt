project(linux_client)

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
        pthread
        )

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
