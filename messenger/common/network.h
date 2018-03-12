#ifndef NETWORK_H_
#define NETWORK_H_

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <netinet/in.h>
#endif

#include <unistd.h>

void network_init();

#endif /* NETWORK_H_ */
