#ifndef MESSENGER_SOCKET_H
#define MESSENGER_SOCKET_H

#include <string>
#include <functional>
#include <sys/socket.h>
#include "DnsFwd.h"

struct Socket {
    using socket_t = int;

    Socket(socket_t fd, sockaddr cli_addr, const std::string &username);
    
    Socket(const std::string &hostname, int port, const std::string &username, int socket_type=SOCK_STREAM);
    
    inline void read_raw(char *buf, size_t size) {
        read(fd, buf, size);
    }
    
    inline void write_raw(const char *buf, size_t size) {
        write(fd, buf, size);
    }
    
    std::uint32_t read_uint() const;
    
    void write_uint(std::uint32_t n) const;
    
    std::string read_string() const;
    
    void write_string(const std::string &str) const;
    
    ~Socket();
    
    static void init();
    
    std::string get_this_username() const {
        return this_username;
    }
    
    std::string get_other_username() const {
        return other_username;
    }

private:
    socket_t fd;
    sockaddr cli_addr;
    std::string this_username = "INVALID_THIS";
    std::string other_username = "INVALID_OTHER";
private:
    static void check_io(ssize_t nbytes, const std::string &process);
    static ssize_t read(socket_t fd, char *buf, size_t size);
    static ssize_t write(socket_t fd, const char *buf, size_t size);
    
//    void init_from_client();
};

#endif //MESSENGER_SOCKET_H
