#ifndef TCP_CLIENT_CLIENT_H
#define TCP_CLIENT_CLIENT_H

#include <WinSock2.h>
#include <thread>

namespace elegram {
    class client {
    public:
        client(const char* addr, uint16_t port, const char* login);
        void connect_to_server();
        void send_message(const char* msg);
        void stop();
        void set_print(bool flag);
        virtual ~client();
    private:
        void _print();
        struct Int {
            union {
                uint32_t value;
                char byte[4];
            };
        } _size;
        int _sockfd;
        uint16_t _portno;
        sockaddr_in _serv_addr;
        hostent* _server;
        char* _login;
        std::thread _client;
        bool _is_start;
        bool _is_print;
    };
}

#endif //TCP_CLIENT_CLIENT_H
