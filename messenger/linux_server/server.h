#pragma once

#define LOCAL_IP "127.0.0.1"
#define MAX_CONNECTIONS 100

#include <iostream>
#include <string>
#include <vector> 
#include <memory>
#include <mutex>
#include <map>
#include <ctime>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "packet_manager.h"

class connection {
public:
    int socket_fb;
	bool is_active;
	packet_manager manager;

    connection(int sock) {
        this->socket_fb = sock;
		this->is_active = true;
	}
};

class server {
	std::vector<std::shared_ptr<connection>> connections;
	std::map<int, std::string> clients;
    std::map<int, std::thread> threads;
	std::mutex connection_mutex;
	int unused_connections = 0;
    sockaddr_in server_addr;
    unsigned int addrlen = sizeof(server_addr);
    int client_sock;
    std::thread serv_thread;

	bool process_message(int id, const std::string& time);

    static void client_handler(int id);
    static void message_sender();

	bool sendall(int id, char* data, int totalbytes);
	void send_str(int id, const std::string& mess);

	bool recvall(int id, char* data, int totalbytes);
	bool get_int(int id, int32_t& value);
	bool get_str(int id, std::string& mess);

	void disconnect_client(int id);
public:
	server(int port_no);
    void close_server();
	bool listen_new_connection();
};

static server* serverptr;
