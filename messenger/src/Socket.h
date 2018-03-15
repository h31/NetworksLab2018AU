#ifndef MESSENGER_SOCKET_H
#define MESSENGER_SOCKET_H

#include <sys/socket.h>
#include "ElegramFwd.h"
#include "Date.h"

#define DEBUG_PROTOCOL 0

struct Socket {
    Socket(int fd, sockaddr cli_addr, unsigned int clilen, const std::string &username);
    
    Socket(const std::string &hostname, int port, const std::string &username);
    
    void write_string(const std::string &str);
    
    void finish();
    
    std::uint32_t read_uint();
    
    std::string read_string();
    
    void write_uint(std::uint32_t n);
    
    Message read_message();
    
    void write_message(const std::string &buffer, const Date &date);
    
    Message read_broadcast();
    
    void write_broadcast(const Message &message);
    
    ~Socket();
    
    int fd = 0;
    sockaddr cli_addr;
    unsigned int clilen;
    std::string this_username = "INVALID_THIS";
    std::string other_username = "INVALID_OTHER";

private:
    static void check_reading(ssize_t nbytes);
};

#endif //MESSENGER_SOCKET_H
