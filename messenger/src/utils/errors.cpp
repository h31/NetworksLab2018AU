#include <iostream>

#include <network.h>
#include <utils/errors.h>

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

#ifdef WIN32
std::string GetLastErrorAsString() {
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}
#endif

void check_error(int n, const std::string str) {
#ifdef WIN32
    if (n == SOCKET_ERROR) {
        std::cerr << GetLastErrorAsString() << ": " << str << "\n";
#else
    if (n < 0) {
        perror(str.c_str());
#endif
        exit(1);
    }
}

void check_error(SOCKET sockfd, const std::string str) {
    if (sockfd == INVALID_SOCKET) {
        check_error(SOCKET_ERROR, str);
    }
}