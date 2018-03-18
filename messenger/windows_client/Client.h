#ifndef MESSENGER_CLIENT_H
#define MESSENGER_CLIENT_H

#include <iostream>
#include <thread>
#include <cstring>
#include "MessagesViewer.h"
#include "Messages.h"

class Client {
public:
	Client(char* hostname, uint16_t port, char * nick) : hostname(hostname), port(port), nick(nick) {
		/* Create a socket point */
		client_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (client_socket < 0) {
			std::cerr << "ERROR opening socket" << std::endl;
			exit(1);
		}
		user_is_typing = new bool();
	}

	void run() {
		hostent * server = gethostbyname(hostname);
		if (!server) {
			std::cerr << "ERROR, no such host\n" << std::endl;
			exit(0);
		}
		sockaddr_in serv_addr{};
		bzero((char *)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy(server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
		serv_addr.sin_port = htons(port);
		/* Now connect to the server */
		if (connect(client_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			perror("ERROR connecting");
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
	int client_socket;
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
