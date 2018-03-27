project(client)

set(HEADERS
        Resolver.h
        )

set(SOURCES
        Resolver.cpp
        main.cpp
        )

set(LIBRARIES
        dnsUtils
        )

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})