project(dnsUtils)

set(HEADERS
        dnsUtils.h
        )

set(SOURCES
        dnsUtils.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ./)