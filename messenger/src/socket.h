#ifndef MESSENGER_SOCKET_H
#define MESSENGER_SOCKET_H

#include <string>
#include <functional>
#if _WIN32
#include <Windows.h>
#else
#include <sys/socket.h>
#endif
#include "elegram_fwd.h"
#include "date.h"


// I/O operations should come through buffer as TCP read/write operations can merge.

struct Socket {
#if _WIN32
    using socket_t = SOCKET;
#else
    using socket_t = int;
#endif

    Socket(socket_t fd, sockaddr cli_addr, const std::string &username);
    
    Socket(const std::string &hostname, int port, const std::string &username);
    
    std::uint32_t read_uint() const;
    
    void write_uint(std::uint32_t n) const;
    
    std::string read_string() const;
    
    void write_string(const std::string &str) const;
    
    Message read_message() const;
    
    void write_message(const std::string &buffer, const Date &date) const;
    
    Message read_broadcast() const;
    
    void write_broadcast(const Message &message) const;
    
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
};

#endif //MESSENGER_SOCKET_H
