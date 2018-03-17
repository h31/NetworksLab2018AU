project(linux_client)

set(HEADERS
        Client.h
        Socket.h
        Message.h
        )

set(SOURCES
        Client.cpp
        Socket.cpp
        Message.cpp
        main.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})