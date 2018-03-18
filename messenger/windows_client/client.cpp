//
// Created by kate on 16.03.18.
//
#define _WIN32_WINNT 0x0501

#include <iostream>
#include <cstring>

#include "../common/client_utils_win.h"

bool is_exited_client = false;
bool is_mode_read = false;

static const char *const THIS_FILE_NAME = \
        strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__;


void print_error(int line, const std::string &mess) {
    std::cerr << THIS_FILE_NAME << "[" << line << "] ERROR: " << mess.c_str() << std::endl;
}

int send_login_to_server(int id_socket, const clien &data_client) {
    return sender(id_socket, data_client, true,
                  std::bind(print_error, __LINE__, std::placeholders::_1));
}

int send_mess_to_server(int id_socket, const clien &data_client) {
    return sender(id_socket, data_client, false,
                  std::bind(print_error, __LINE__, std::placeholders::_1));
}


void *reader_message(void *id_server) {

    while (!is_exited_client) {
        uint16_t id_socket = *(uint16_t *) id_server;
        clien date_client;
        int n = reader(id_socket, date_client, is_exited_client,
                       std::bind(print_error, __LINE__, std::placeholders::_1));
        if (n == 0) {
            while (is_mode_read);
            std::cout << date_client << std::endl;
        }
    }
	return nullptr;
}


void set_line(clien &data_client, char *mess) {
    data_client.mess.size = strlen(mess);
    data_client.mess.cap = strlen(mess) + 1;
    data_client.mess.mess = new char[sizeof(char) * data_client.mess.cap];
    memcpy(data_client.mess.mess, mess, data_client.mess.size);
    data_client.mess.mess[data_client.mess.size] = '\0';
    get_current_time(data_client.time);
}

void get_argv(const int argc,char **argv, char **address_server,
              uint16_t &port_server, char **nick_name) {

    int res;
	
	size_t len = strlen(argv[1]);
	*address_server = new char[len + 1];
	strncpy(*address_server, argv[1], len);
	(*address_server)[len]= '\0';

	// server port
	port_server = (uint16_t)(atoi(argv[2]));

	// nick name
	len = strlen(argv[3]);
	*nick_name =new char[len + 1];
	strncpy(*nick_name, argv[3], len);
	(*nick_name)[len] = '\0';
}

void logic(int id_socket, clien & data_client) {
	int res = send_login_to_server(id_socket, data_client);
	if (res != 0) {
		exit(1);
	}
	pthread_t updates_thread;
	int code = pthread_create(&updates_thread, nullptr, reader_message,
		&id_socket);
	if (code < 0) {
		print_error(__LINE__, "on creating reader thread");
		exit(1);
	}

	std::string specification = ":m - write message\n:q - exit";

	std::cout << specification.c_str() << std::endl;

	char line[256];
	while (!is_exited_client) {
		std::cin.getline(line, 256);
		std::string m(line);
		if (m.find(":m") == 0) {
			is_mode_read = true;
			std::cout << " get line " << line + 2 << "  " << strlen(line) << std::endl;
			set_line(data_client, line + 2);
			is_mode_read = false;
			send_mess_to_server(id_socket, data_client);

		}
		else {
			if (m.find(":q") == 0) {
				is_exited_client = true;

				data_client.set_empty();
				send_login_to_server(id_socket, data_client);
				continue;
			}
		}
	}
}


int main(int argc, char *argv[]) {

    using namespace std;
    if (argc < 3) {
        std::cout << " put param -s server_adde -p port_server -n nick_name" 
			<< std::endl;
        return 1;
    }

    char *address_server;
    uint16_t port_server;
    char *nick_name;

    get_argv(argc, argv, &address_server, port_server, &nick_name);

    clien data_client(nick_name);
	std::cout << " get client  " << data_client << std::endl;

	// initialize winsock
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		print_error(__LINE__ , "Failed. Error Code : " + WSAGetLastError());
		return 1;
	}

	struct addrinfo *result = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char server_port[6];
	snprintf(server_port, 6, "%u", port_server);

	if (getaddrinfo(address_server, server_port, &hints, &result) != 0) {
		print_error(__LINE__, "getaddrinfo failed");
		WSACleanup();
		return 1;
	}

	SOCKET id_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (id_socket == INVALID_SOCKET) {
		print_error(__LINE__, "Error at socket(): " + WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	if (connect(id_socket, result->ai_addr, (int)result->ai_addrlen)) {
		closesocket(id_socket);
		freeaddrinfo(result);
		return 1;
	}
	freeaddrinfo(result);

	logic(id_socket, data_client);
    
	if (shutdown(id_socket, SD_BOTH) == SOCKET_ERROR) {
		print_error(__LINE__, "shutdown failed: " + WSAGetLastError());
		closesocket(id_socket);
		WSACleanup();
		return 1;
	}
	closesocket(id_socket);
	WSACleanup();

    return 0;
}