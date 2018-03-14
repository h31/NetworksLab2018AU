#pragma once

const char *ACCEPT_ERROR         = "ERROR on accept";

const char *SOCKET_OPEN_ERROR    = "ERROR opening socket";
const char *SOCKET_BIND_ERROR    = "ERROR binding socket";
const char *SOCKET_WRITE_ERROR   = "ERROR writing to socket";
const char *SOCKET_READ_ERROR    = "ERROR reading from socket";

const char *CONNECT_ERROR        = "ERROR could not connect";
const char *DISCONNECT_ERROR     = "ERROR closing socket";

const char *UNKNOWN_PACKET_ERROR = "ERROR unknown packet type";
const char *NOT_LOGGED_IN_ERROR  = "ERROR not logged in";

void check_error(int, const char *);
