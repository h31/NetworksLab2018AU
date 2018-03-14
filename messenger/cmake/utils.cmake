add_library(utils STATIC
    src/utils/errors.cpp
    src/utils/packets.cpp 
)

target_include_directories(utils
    include
)