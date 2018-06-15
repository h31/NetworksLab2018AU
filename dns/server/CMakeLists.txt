cmake_minimum_required(VERSION 3.10)
project(dns_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(server main.cpp server.cpp ../dns_packet.cpp ../dns_packet.h ../resource_record.cpp ../resource_record.h ../query.cpp ../query.h ../header.cpp)
