cmake_minimum_required(VERSION 2.8)
project(server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp server.cpp server.h main.cpp ../header.cpp ../header.h ../query.h ../query.cpp ../resource_record.h ../resource_record.cpp ../dns_packet.h ../dns_packet.cpp ../name.h ../name.cpp)
add_executable(server ${SOURCE_FILES})
