#ifndef MESSENGER_SOCKET_H
#define MESSENGER_SOCKET_H

#include <sys/socket.h>
#include "ElegramFwd.h"
#include "Date.h"

// I/O operations should come through buffer as TCP read/write operations can merge.

struct Socket {
    Socket(int fd, sockaddr cli_addr, unsigned int clilen, const std::string &username);
    
    Socket(const std::string &hostname, int port, const std::string &username);
    
    void finish();
    
    std::uint32_t read_uint() const;
    
    void write_uint(std::uint32_t n) const;
    
    std::string read_string() const;
    
    void write_string(const std::string &str) const;
    
    Message read_message() const;
    
    void write_message(const std::string &buffer, const Date &date) const;
    
    Message read_broadcast() const;
    
    void write_broadcast(const Message &message) const;
    
    ~Socket();
    
    int fd = 0;
    sockaddr cli_addr;
    unsigned int clilen;
    std::string this_username = "INVALID_THIS";
    std::string other_username = "INVALID_OTHER";

private:
    static void check_io(ssize_t nbytes, const std::string &process);
};

#endif //MESSENGER_SOCKET_H
