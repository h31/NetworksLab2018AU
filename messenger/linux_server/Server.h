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
        messages = new MessagesQueue();
        connected_clients_sockets = new std::vector<int>();
        sockaddr_in serv_addr{};

        /* First call to socket() function */
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket < 0) {
            std::cerr << "ERROR opening socket" << std::endl;
            exit(1);
        }

        /* Initialize socket structure */
        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        /* Now bind the host address using bind() call.*/
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
        while(true) {
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
    }
    
    ~Server() {
        delete message_sender;
        delete messages;
        delete connected_clients_sockets;
        close(server_socket);
        for (ClientHandler & client_handler : client_handlers) {
            client_handler.stop();
        }
    }
private:
    uint16_t port;
    int server_socket;
    MessageSender * message_sender;
    std::vector<ClientHandler> client_handlers;
    MessagesQueue * messages;
    std::vector<int> * connected_clients_sockets;
    ClientHandler * clientHandler1;
    ClientHandler * clientHandler2;
    ClientHandler * clientHandler3;
};


#endif //MESSENGER_SERVER_H
