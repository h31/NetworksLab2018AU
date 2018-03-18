#ifndef MESSENGER_SERVER_H
#define MESSENGER_SERVER_H


#include <stdint-gcc.h>
#include <netinet/in.h>
#include <iostream>
#include <strings.h>
#include "MessageSender.h"
#include "ClientHandler.h"

class Server {
public:
    Server(uint16_t port, int threads_number) : port(port) {
        client_handlers.reserve(threads_number);
        messages = new Messages();
        connected_clients_sockets = new std::vector<int>();
        sockaddr_in serv_addr{};

        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket < 0) {
            std::cerr << "ERROR opening socket" << std::endl;
            exit(1);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        if (bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
            exit(1);
        }
    }
    
    void run() {
        sockaddr_in cli_addr{};
        listen(server_socket, 5);
        unsigned int clilen = sizeof(cli_addr);

        message_sender = new MessageSender(messages, connected_clients_sockets);
        int client = 0;
        while(!stopped) {
            int newsockfd = accept(server_socket, (struct sockaddr *) &cli_addr, &clilen);
            client_handlers[client] = ClientHandler(messages, newsockfd);
            client_handlers[client].run();
            if (newsockfd < 0) {
                perror("ERROR on accept");
                exit(1);
            }
            connected_clients_sockets->push_back(newsockfd);
            client++;
        }
        close(server_socket);
        for (auto client_socket : *connected_clients_sockets) {
            close(client_socket);
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
    int server_socket;
    MessageSender * message_sender;
    std::vector<ClientHandler> client_handlers;
    Messages * messages;
    std::vector<int> * connected_clients_sockets;
    bool stopped = false;
};


#endif //MESSENGER_SERVER_H
