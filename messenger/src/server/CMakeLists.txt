project(server)
find_package (Threads)

set(SOURCE_FILES
        ${DIR_INC}/socket.hpp
        sever.cpp)
include_directories(${DIR_INC})

add_executable(server ${SOURCE_FILES})
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
  target_link_libraries(server wsock32 ws2_32)
endif()
