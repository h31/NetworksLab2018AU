#include "server.h"
#include "message.h"
#include <ctime>

elegram::server::server(uint16_t port)
    : _portno(port) {
    bzero((char *) &_serv_addr, sizeof(_serv_addr));
    _serv_addr.sin_family = AF_INET;
    _serv_addr.sin_addr.s_addr = INADDR_ANY;
    _serv_addr.sin_port = htons(_portno);
}

void elegram::server::start() {
    _is_start = true;
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        throw std::runtime_error("ERROR opening socket");
    }
    if (bind(_sockfd, (sockaddr *) &_serv_addr, sizeof(_serv_addr)) < 0) {
        throw std::runtime_error("ERROR on binding");
    }
    listen(_sockfd, 5);
    while (_is_start) {
        unsigned int clilen;
        sockaddr_in cli_addr{};
        clilen = sizeof(cli_addr);
        _newsockfd = accept(_sockfd, (sockaddr *) &cli_addr, &clilen);
        std::unique_lock<std::mutex> lock(_sockets_mutex);
        _sockets.push_back(_newsockfd);
        lock.unlock();
        _client_routine.push_back(std::thread(&server::_client, this, _newsockfd));
    }
}

elegram::server::~server() {
    if (_is_start) {
        stop();
    }
}

void elegram::server::stop() {
    _is_start = false;
    for (size_t i = 0; i < _sockets.size(); ++i) {
        shutdown(_sockets[i], SHUT_RDWR);
        _client_routine[i].join();
    }
}

void elegram::server::_client(int socket) {
    message m(socket);
    time_t now = time(0);
    while(m.has_next()) {
        auto login = m.get_login();
        auto msg = m.get_message();
        int size_write_msg = static_cast<int>(strlen(login) + 1 + strlen(msg) + 1 + 4 * sizeof(int));
        char* msg_send = new char[size_write_msg];

        tm *ltm = localtime(&now);

        uint32_t offset = 4;
        Int hours;
        hours.value = ltm->tm_hour;
        msg_send[0] = hours.byte[0];
        msg_send[1] = hours.byte[1];
        msg_send[2] = hours.byte[2];
        msg_send[3] = hours.byte[3];
        Int min;
        min.value = ltm->tm_min;
        msg_send[offset + 0] = min.byte[0];
        msg_send[offset + 1] = min.byte[1];
        msg_send[offset + 2] = min.byte[2];
        msg_send[offset + 3] = min.byte[3];
        offset += 4;
        Int login_size;
        login_size.value = strlen(login) + 1;
        msg_send[offset + 0] = login_size.byte[0];
        msg_send[offset + 1] = login_size.byte[1];
        msg_send[offset + 2] = login_size.byte[2];
        msg_send[offset + 3] = login_size.byte[3];
        offset += 4;
        strcpy(msg_send + offset, login);
        offset += strlen(login) + 1;
        Int msg_size;
        msg_size.value = strlen(msg) + 1;
        msg_send[offset + 0] = msg_size.byte[0];
        msg_send[offset + 1] = msg_size.byte[1];
        msg_send[offset + 2] = msg_size.byte[2];
        msg_send[offset + 3] = msg_size.byte[3];
        offset += 4;
        strcpy(msg_send + offset, msg);

        ssize_t n;
        std::unique_lock<std::mutex> lock(_sockets_mutex);
        for (int i = 0; i < _sockets.size(); ++i) {
            n = write(_sockets[i], msg_send, size_write_msg);
        }
        lock.unlock();
        if (n < 0) {
            throw std::runtime_error("ERROR writing to socket");
        }
        delete []msg_send;
    }
    std::unique_lock<std::mutex> lock(_sockets_mutex);
    _sockets.erase(_sockets.find(socket));
}
