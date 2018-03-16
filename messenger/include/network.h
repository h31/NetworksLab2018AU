#pragma once

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>

    #define poll WSAPoll
    #define POLLFD WSAPOLLFD
#define close closesocket
#else
    #include <netdb.h>
    #include <netinet/in.h>
    #include <unistd.h>

    #define SOCKET int
    #define POLLFD pollfd
#endif
