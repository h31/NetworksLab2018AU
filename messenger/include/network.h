#pragma once

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>

    #define poll WSAPoll
    #define POLLFD WSAPOLLFD
    #define close closesocket
    #define socklen_t int
#else
    #include <netdb.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <sys/poll.h>

    #define SOCKET int
    #define POLLFD pollfd
    #define ADDRINFOA addrinfo
#endif
