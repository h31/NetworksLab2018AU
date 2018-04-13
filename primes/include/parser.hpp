#ifndef HTTP_HPP_
#define HTTP_HPP_

#include <ext/stdio_filebuf.h>
#include <iostream>
#include <fstream>
#include <string>

#include "protocol.hpp"
#include "socket.hpp"

struct parser
{
    net::socket_stream& sock;

public:
    explicit parser(net::socket_stream& _sock)
        : sock(_sock)
    {}

    void check_conn() const {
        sock.check_conn();
    }
};


#endif // HTTP_HPP
