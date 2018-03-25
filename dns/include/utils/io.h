#pragma once

#include <string>

#include <network.h>
#include <dns.h>

namespace io {

class io_exception {
    int errcode;
    const std::string additional_message;
public:
    io_exception(int, std::string);
    void show_error();
};

void write_message(SOCKET, dns::message, sockaddr *, socklen_t);

template<typename T>
inline T read_data(char *buf, int &bufpos) {
    T result = *((T *) (buf + bufpos));
    bufpos += sizeof(T);
    return result;
}

std::string read_name(char *, int &, char *);
dns::message read_message(SOCKET, sockaddr *, socklen_t *);

} // namespace io