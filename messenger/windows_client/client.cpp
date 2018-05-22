#include <cstdlib>
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"

elegram::client::client(const char *addr, uint16_t port, const char* login)
    : _portno(port)
    , _is_start(true)
    , _is_print(true) {
	WSADATA ws;
	WSAStartup(MAKEWORD(1, 1), &ws);
    _login = new char[strlen(login) + 1];
    strcpy(_login, login);
    _server = gethostbyname(addr);
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        throw std::runtime_error("ERROR opening socket");
    }
	memset((char *)&_serv_addr, 0, sizeof(_serv_addr));
    _serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.S_un.S_addr = inet_addr(addr);
    _serv_addr.sin_port = htons(_portno);
}

void elegram::client::connect_to_server() {
    if (connect(_sockfd, (struct sockaddr *) &_serv_addr, sizeof(_serv_addr)) < 0) {
        throw std::runtime_error("ERROR connecting");
    }
    _client = std::move(std::thread(&client::_print, this));
}

void elegram::client::send_message(const char *msg) {
    auto buffer_size = static_cast<uint32_t>(strlen(msg) + strlen(_login) + 2 + 2 * sizeof(uint32_t));
    auto* buffer = new char[buffer_size]{};
    _size.value = static_cast<uint32_t>(strlen(_login) + 1);
    uint32_t offset = 4;
    buffer[0] = _size.byte[0];
    buffer[1] = _size.byte[1];
    buffer[2] = _size.byte[2];
    buffer[3] = _size.byte[3];
    strcpy(buffer + offset, _login);
    offset += strlen(_login) + 1;
    _size.value = static_cast<uint32_t>(strlen(msg) + 1);
    buffer[offset + 0] = _size.byte[0];
    buffer[offset + 1] = _size.byte[1];
    buffer[offset + 2] = _size.byte[2];
    buffer[offset + 3] = _size.byte[3];
    offset += 4;
    strcpy(buffer + offset, msg);
    auto n = send(_sockfd, buffer, buffer_size, 0);
    if (n < 0) {
        std::runtime_error("ERROR write");
    }
    delete []buffer;
}

void elegram::client::_print() {
    while(_is_start) {
        Int hours;
        auto n = recv(_sockfd, hours.byte, 4, 0);
        Int minutes;
        n = recv(_sockfd, minutes.byte, 4, 0);
        Int login_size;
        n = recv(_sockfd, login_size.byte, 4, 0);
        char* login = new char[login_size.value];
        n = recv(_sockfd, login, login_size.value, 0);
        Int msg_size;
        n = recv(_sockfd, msg_size.byte, 4, 0);
        char* msg = new char[login_size.value];
        n = recv(_sockfd, msg, msg_size.value, 0);
        if (_is_print) {
            std::cout << "<" << hours.value << ":" << minutes.value << "> [" << login << "] " << msg << std::endl;
        }
    }
}

void elegram::client::stop() {
    _is_start = false;
    _client.join();
    delete []_login;
}

elegram::client::~client() {
    if (_is_start) {
        stop();
    }
}

void elegram::client::set_print(bool flag) {
    _is_print = flag;
}
