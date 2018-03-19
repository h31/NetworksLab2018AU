#pragma once


#include <iostream>
#include <string>
#include <queue>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

class client {
    int socket_fb;
    sockaddr_in server_addr;
    unsigned int sizeofaddr = sizeof(server_addr);
	std::string name;
	bool is_input = false;
	std::queue<std::string> message_queue;
    std::thread client_thread;

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
