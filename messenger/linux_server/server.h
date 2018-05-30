#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include "vector.h"

namespace elegram {
    class server {
    public:
        explicit server(uint16_t port);
        void start();
        void stop();
        virtual ~server();
    private:
        void _client(int socket);
        struct Int {
            union {
                uint32_t value;
                char byte[4];
            };
        };
        int _sockfd;
        int _newsockfd;
        uint16_t _portno;
        sockaddr_in _serv_addr;
        vector<int> _sockets;
        std::mutex _sockets_mutex;
        vector<std::thread> _client_routine;
        bool _is_start;
    };
}


#endif //TCP_SERVER_H
