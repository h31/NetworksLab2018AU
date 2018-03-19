#ifndef MESSENGER_SERVER_H
#define MESSENGER_SERVER_H

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include "MessageSender.h"
#include "ClientHandler.h"

class Server {
public:
    Server(uint16_t port, int threads_number) : port(port) {
        // client_handlers.reserve(threads_number);
        messages = new Messages();
        connected_clients_sockets = new std::vector<int>();
        SOCKADDR_IN serv_addr;

        if (server_socket < 0) {
            std::cerr << "ERROR opening socket" << std::endl;
            exit(1);
        }

		WSAData wsa;
		WORD dll_ver = MAKEWORD(2, 1);
		if (WSAStartup(dll_ver, &wsa) != 0) {
			MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
			exit(1);
		}

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

		server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (bind(server_socket, (SOCKADDR*) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
            exit(1);
        }
    }
    
    void run() {
        SOCKADDR_IN cli_addr;
        listen(server_socket, 5);
        int clilen = sizeof(cli_addr);

        message_sender = new MessageSender(messages, connected_clients_sockets);
        int client = 0;
        while(!stopped) {
            SOCKET newsockfd = accept(server_socket, (SOCKADDR*) &cli_addr, &clilen);
			client_handlers.push_back(ClientHandler(messages, newsockfd));
            client_handlers.back().run();
            if (newsockfd == 0) {
                perror("ERROR on accept");
                exit(1);
            }
            connected_clients_sockets->push_back(newsockfd);
            client++;
        }
        closesocket(server_socket);
        for (auto client_socket : *connected_clients_sockets) {
            closesocket(client_socket);
        }
    }
    
    void stop() {
        stopped = true;
    }
    
    ~Server() {
        delete message_sender;
        delete messages;
        delete connected_clients_sockets;
        for (ClientHandler & client_handler : client_handlers) {
            client_handler.stop();
        }
    }
private:
    uint16_t port;
    SOCKET server_socket;
    MessageSender * message_sender;
    std::vector<ClientHandler> client_handlers;
    Messages * messages;
    std::vector<int> * connected_clients_sockets;
    bool stopped = false;
};


#endif //MESSENGER_SERVER_H
