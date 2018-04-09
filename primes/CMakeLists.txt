cmake_minimum_required(VERSION 2.6)
project(primes)

set(DIR_ROOT ${PROJECT_SOURCE_DIR})
set(DIR_SRC ${DIR_ROOT}/src)
set(DIR_INC ${DIR_ROOT}/include)
set(DIR_BIN ${DIR_ROOT}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_BIN})

add_subdirectory(src/server)
add_subdirectory(src/client)
