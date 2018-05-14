#ifndef MESSENGER_SERVERSOCKET_H
#define MESSENGER_SERVERSOCKET_H

#include "elegram_fwd.h"

struct ServerSocket {
    explicit ServerSocket(int portno);
    
    ~ServerSocket();
    
    /**
     * Now start listening for the clients, here process will
     * go in sleep mode and will wait for the incoming connection
     * @param nrequests
     */
    void listen(int nrequests=5);
    
    SocketWrapper accept();

private:
    int fd = 0;
};


#endif //MESSENGER_SERVERSOCKET_H
