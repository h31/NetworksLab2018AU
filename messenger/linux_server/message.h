#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "vector.h"

namespace elegram {
    class message {
    public:
        explicit message(int socket);

        char* get_login();

        char* get_message();

        bool has_next();
    private:
        struct Int {
            union {
                uint32_t value;
                char byte[4];
            };
        } _size;
        char* _read_n(int n);
        int _socket;
        static const int _BUFFSER_SIZE = 256;
        char _buffer[_BUFFSER_SIZE];
        char* _login;
        char* _message;
    };
}


#endif //TCP_MESSAGE_H
