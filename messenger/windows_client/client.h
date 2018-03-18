#pragma once

#pragma comment(lib, "ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <string>
#include <queue>
#include <WinSock2.h>

class client {
	SOCKET sock;				
	SOCKADDR_IN client_addr;			
	int sizeofaddr = sizeof(client_addr);
	std::string name;
	bool is_input = false;
	std::queue<std::string> message_queue;

	bool process_message();
	static void client_handler();

	bool sendall(char* data, int totalbytes);
	bool send_int(int32_t value);

	bool recvall(char* data, int totalbytes);
	bool get_int(int32_t& value);
	bool get_str(std::string& mess);

public:
	client(std::string ip, int port_no, const std::string& name);
	bool connect_to_server();
	bool disconnet();
	bool send_str(const std::string& mess);
	void set_input_mode();
};

static client* clientptr;
