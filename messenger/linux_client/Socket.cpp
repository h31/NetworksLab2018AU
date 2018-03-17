#include "Socket.h"

Socket::Socket(const std::string &host, int port) : host(host), port(port) {}

Message Socket::read() {
    return Message();
}

void Socket::write(const Message &message) {

}
