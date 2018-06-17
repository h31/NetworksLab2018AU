#ifndef SERVER
#define SERVER

#include "../header.h"
#include <map>
#include <arpa/inet.h>

class server {
    int client_sock;
    struct sockaddr_in server_addr = {};
    unsigned int addrlen = sizeof(server_addr);
    std::map <std::string, std::string> hosts;
    char* buffer = nullptr;

public:
    explicit server(std::string const& hosts_list, int port = 1153);

    ~server();

    void run();
};

#endif // SERVER

