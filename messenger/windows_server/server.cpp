//
// Created by kate on 16.03.18.
//

#define _WIN32_WINNT 0x0501

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

#include "../common/client_utils_win.h"

bool is_exited_client = false;
bool is_mode_read = false;

static const char *const THIS_FILE_NAME =  strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__;

pthread_mutex_t mutex_client = PTHREAD_MUTEX_INITIALIZER;

std::vector<int> socket_idx;
int count_client = 0;


void print_error(int line, const std::string &mess) {
    char time[80];
    get_current_time(time);
    std::cerr << time << ":" << THIS_FILE_NAME << " [" << line << "] ERROR: " << mess.c_str() << std::endl;
}

void add_new_client(int id_socket) {
    pthread_mutex_lock(&mutex_client);
    count_client++;
    socket_idx.push_back(id_socket);
    pthread_mutex_unlock(&mutex_client);
}

void remove_client(int id_socket) {
    pthread_mutex_lock(&mutex_client);
    socket_idx.erase(
            std::remove(socket_idx.begin(), socket_idx.end(), id_socket),
            socket_idx.end());
    pthread_mutex_unlock(&mutex_client);
}


int write_message(const clien &client_data) {
    using namespace std::placeholders;
    pthread_mutex_lock(&mutex_client);
    for (auto item : socket_idx) {
        int n = sender(item, client_data, false,
                       std::bind(print_error, __LINE__, _1));
        if (n < 0) {
            print_error(__LINE__, "write to socket " + item);
            continue;
        }
    }
    pthread_mutex_unlock(&mutex_client);


    return 0;
}

int read_message(int id_socket, clien &clien_data, bool exit_client = false) {
    return reader(id_socket, clien_data, exit_client, 
                  std::bind(print_error, __LINE__, std::placeholders::_1));
}

void *handle_client(void *client_socket) {

    uint16_t id_socket = *(uint16_t *) client_socket;

    add_new_client(id_socket);

    clien clien_data;
    if (read_message(id_socket, clien_data) < 0) {
        print_error(__LINE__, " can not read login client; connection failed");
        return nullptr;
    }

    while (true) {
        int res = read_message(id_socket, clien_data);
        if (res < 0) {
            print_error(__LINE__, "not read message from client");
            continue;
        }
        if (res == 1) {
            // exit mess
            remove_client(id_socket);
            return nullptr;
        } else {
            std::cout << clien_data << std::endl;
            write_message(clien_data);
        }
    }

}

int run_server(int port) {

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		print_error(__LINE__, "ERROR: initialize WSAStartup");
		return 1;
	}

	/* Initialize socket structure */
	struct addrinfo *result = nullptr, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	char server_port[6];
	snprintf(server_port, 6, "%u", port);

	std::cout << getaddrinfo(NULL, server_port, &hints, &result) << std::endl;
	if (getaddrinfo(NULL, server_port, &hints, &result) != 0) {
		print_error(__LINE__, "getaddrinfo failed");
		WSACleanup();
		return 1;
	}

	int id_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (id_socket == INVALID_SOCKET) {
		print_error(__LINE__, "Error at socket(): " + WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	/* Now bind the host address using bind() call.*/
	if (bind(id_socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		print_error(__LINE__, "bind failed with error " + WSAGetLastError());
		freeaddrinfo(result);
		closesocket(bind(id_socket, result->ai_addr, (int)result->ai_addrlen));
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);


	if (listen(id_socket, SOMAXCONN) == SOCKET_ERROR) {
		print_error(__LINE__, "Listen failed with error " + WSAGetLastError());
		closesocket(id_socket);
		WSACleanup();
		return 1;
	}

    struct sockaddr_in cli_addr{};
    unsigned int clilen = sizeof(cli_addr);


    while (true) {
        int client_socket = accept(id_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET) {
			print_error(__LINE__, "accept failed " + WSAGetLastError());
			closesocket(id_socket);
			WSACleanup();
			return 1;
		}

        pthread_t client_thread;
        int id_thread = pthread_create(&client_thread, NULL, handle_client,
                                       &client_socket);

        if (id_thread < 0) {
            print_error(__LINE__, "can not start  thread for client");
        }
    }


}


int main(int argc, char *argv[]) {

    using namespace std;
    if (argc < 2) {
        cout << " args:  port " << endl;
        return 1;
    }

    int port_server;
	port_server = (uint16_t)(atoi(argv[2]));

    return run_server(port_server);
}