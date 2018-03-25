#pragma once

#include <string>

extern const std::string ADDRINFO_ERROR;
extern const std::string ACCEPT_ERROR;
extern const std::string SOCKET_OPEN_ERROR;
extern const std::string SOCKET_CLOSE_ERROR;
extern const std::string SOCKET_BIND_ERROR;
extern const std::string SOCKET_WRITE_ERROR;
extern const std::string SOCKET_READ_ERROR;
extern const std::string CONNECT_ERROR;
extern const std::string UNKNOWN_PACKET_ERROR;
extern const std::string NOT_LOGGED_IN_ERROR;
extern const std::string POLL_ERROR;

void check_error(int, const std::string);