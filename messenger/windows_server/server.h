#pragma once

#pragma comment(lib, "ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LOCAL_IP "127.0.0.1"
#define MAX_CONNECTIONS 100

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <string>
#include <vector> 
#include <memory>
#include <mutex>
#include <map>
#include <ctime>
#include <WinSock2.h>
#include "packet_manager.h"

class connection {
public:
	SOCKET socket;
	bool is_active;
	packet_manager manager;

	connection(SOCKET socket) {
		this->socket = socket;
		this->is_active = true;
	}
};

class server {
	std::vector<std::shared_ptr<connection>> connections;
	std::map<int, std::string> clients;
	std::mutex connection_mutex;
	int unused_connections = 0;
	SOCKADDR_IN server_addr; 			
	int addrlen = sizeof(server_addr);	
	SOCKET client_sock;

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
	bool listen_new_connection();
};

static server* serverptr;
