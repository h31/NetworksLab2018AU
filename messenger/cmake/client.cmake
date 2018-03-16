add_executable(client src/client/main.cpp)

target_include_directories(client
    PUBLIC include
)

target_link_libraries(client
    PRIVATE utils
)