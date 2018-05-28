#ifndef MESSENGER_CLIENT_H
#define MESSENGER_CLIENT_H

#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <thread>
#include <cstring>
#include <WinSock2.h>
#include <memory.h>
#include "MessagesViewer.h"
#include "Messages.h"

class Client {
public:
	Client(char* hostname, uint16_t port, char * nick) : hostname(hostname), port(port), nick(nick) {
		user_is_typing = new bool();
	}

	void run() {
		SOCKADDR_IN serv_addr;

		WSAData wsa;
		WORD dll_ver = MAKEWORD(2, 1);
		if (WSAStartup(dll_ver, &wsa) != 0) {
			MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
			exit(1);
		}

		/* hostent * server = gethostbyname(hostname);
		if (!server) {
		std::cerr << "ERROR, no such host\n" << std::endl;
		exit(0);
		}*/
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(hostname);
		serv_addr.sin_port = htons(port);
		
		/* Create a socket point */
		client_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (client_socket == 0) {
			std::cerr << "ERROR opening socket" << std::endl;
			exit(1);
		}

		/* Now connect to the server */
		int iResult = connect(client_socket, (SOCKADDR*)&serv_addr, sizeof(serv_addr));
		if (iResult < 0) {
			std::cerr << "ERROR connecting: " << WSAGetLastError() << std::endl;
			exit(1);
		}

		Message nick_message(nick, "");
		nick_message.write_to_socket(client_socket);

		messages_queue = new Messages();
		MessagesViewer messages_viewer(messages_queue, user_is_typing);
		std::thread user_input_reader(&Client::read_user_input, this, client_socket);


		while (!stopped) {
			while (!(*user_is_typing) && !stopped) {
				messages_queue->read_and_push(client_socket);
			}
		}

		user_input_reader.join();
		messages_viewer.stop();
	}

	~Client() {
		delete messages_queue;
		delete user_is_typing;
	}

private:
	SOCKET client_socket;
	Messages * messages_queue;
	char *hostname;
	uint16_t port;
	char * nick;
	bool * user_is_typing;
	bool stopped = false;


	void read_user_input(int socket) {
		std::string input;
		while (!stopped) {
			std::cin >> input;
			if (input == "m") {
				*user_is_typing = true;
				std::cout << "Please enter the message: ";
				std::cin >> input;
				Message new_message(nick, input);
				new_message.write_to_socket(client_socket);
			}
			else if (input == "s") {
				stopped = true;
				Message stop_message(nick, "#STOP");
				stop_message.write_to_socket(client_socket);
			}
			else {
				std::cout << "Usage:\n m - new message \n s - stop chating" << std::endl;
			}
			*user_is_typing = false;
		}
	}
};


#endif //MESSENGER_CLIENT_H
