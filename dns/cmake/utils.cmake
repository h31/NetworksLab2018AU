add_library(utils STATIC
    src/utils/errors.cpp
    src/utils/io.cpp
)

target_include_directories(utils
    PUBLIC include
)

target_link_libraries(utils
    pthread
)
