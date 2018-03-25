#include <iostream>

#include <network.h>
#include <utils/errors.h>

const std::string ADDRINFO_ERROR       = "ERROR getting host info";
const std::string ACCEPT_ERROR         = "ERROR on accept";
const std::string SOCKET_OPEN_ERROR    = "ERROR opening socket";
const std::string SOCKET_CLOSE_ERROR   = "ERROR closing socket";
const std::string SOCKET_BIND_ERROR    = "ERROR binding socket";
const std::string SOCKET_WRITE_ERROR   = "ERROR writing to socket";
const std::string SOCKET_READ_ERROR    = "ERROR reading from socket";
const std::string CONNECT_ERROR        = "ERROR could not connect";
const std::string UNKNOWN_PACKET_ERROR = "ERROR unknown packet type";
const std::string NOT_LOGGED_IN_ERROR  = "ERROR not logged in";
const std::string POLL_ERROR           = "ERROR poll completed with an error";

void check_error(int n, const std::string str) {
    if (n < 0) {
        perror(str.c_str());
        exit(1);
    }
}