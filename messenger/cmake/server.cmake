add_executable(server
    src/server/main.cpp
)

target_include_directories(server
    PUBLIC include
)

target_link_libraries(server
    utils
    pthread
)