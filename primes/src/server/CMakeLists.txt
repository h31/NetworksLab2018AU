project(server)
find_package (Threads)

set(SOURCE_FILES
        ${DIR_INC}/socket.hpp
        ${DIR_INC}/protocol.hpp
        ${DIR_INC}/parser.hpp
        ${DIR_INC}/list.h
        Server.cpp)
include_directories(${DIR_INC})

add_executable(server ${SOURCE_FILES})
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
