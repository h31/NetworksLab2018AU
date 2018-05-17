project(client)
find_package (Threads)

set(SOURCE_FILES
        ${DIR_INC}/dns.h
        client.c)
include_directories(${DIR_INC})


add_executable(client ${SOURCE_FILES})
target_link_libraries (client ${CMAKE_THREAD_LIBS_INIT})
