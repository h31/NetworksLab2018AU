#include "network.h"

#include <stdio.h>
#include <signal.h>

void disable_broken_pipe() {
    #ifndef _WIN32
        struct sigaction new_actn, old_actn;
        new_actn.sa_handler = SIG_IGN;
        sigemptyset(&new_actn.sa_mask);
        new_actn.sa_flags = 0;
        sigaction(SIGPIPE, &new_actn, &old_actn);
    #endif
}

void network_init() {
    #ifdef _WIN32
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;

        int err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) {
            printf("WSAStartup failed with error: %d\n", err);
            exit(1);
        }
    #else
        disable_broken_pipe();
    #endif
}
