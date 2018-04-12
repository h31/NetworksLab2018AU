#ifndef __SERVER_H__
#define __SERVER_H__

#include "Resolver.h"

#define PACKET_SIZE 512

class Server
{
public:
    void init(const std::string &hosts_file_name);
    void open_sockets (uint16_t port);
    void run();
    ~Server() {
        delete resolver;
    }
private:
    int listenfd;
    Resolver* resolver;
};

#endif
