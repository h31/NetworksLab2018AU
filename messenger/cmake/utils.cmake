add_library(utils STATIC
    src/utils/errors.cpp
    src/utils/packets.cpp
)

target_include_directories(utils
    PUBLIC include
)

if (WIN32)
    target_link_libraries(utils
        Ws2_32
    )
else()
    target_link_libraries(utils
        pthread
    )
endif()