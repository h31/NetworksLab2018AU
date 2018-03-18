#ifndef MESSENGER_CLIENT_H
#define MESSENGER_CLIENT_H

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <strings.h>
#include <cstring>
#include "MessagesViewer.h"

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
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
        serv_addr.sin_port = htons(port);
        /* Now connect to the server */
        if (connect(client_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(1);
        }

        /* Send message to the server */
        ssize_t n = write(client_socket, nick, strlen(nick));

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        auto * messages_queue = new MessagesQueue();
        MessagesViewer messagesViewer(messages_queue, user_is_typing);
        std::thread user_input_reader(&Client::read_user_input, this, client_socket);
        

        while (true) {
            while (!(*user_is_typing)) {
                char buffer[1];
                bzero(buffer, 1);
                ssize_t n = read(client_socket, buffer, 1);

                if (n < 0) {
                    perror("ERROR reading from socket");
                    exit(1);
                }
                int8_t nicklen = buffer[0];
                char nick_buffer[nicklen];
                n = read(client_socket, nick_buffer, nicklen);

                if (n < 0) {
                    perror("ERROR reading from socket");
                    exit(1);
                }
                std::string nick;
                for (unsigned int charno = 0; charno < nicklen; charno++) {
                    nick.push_back(nick_buffer[charno]);
                }
                
                char text_len_buffer[5];
                /* If connection is established then start communicating */
                bzero(text_len_buffer, 5);
                n = read(client_socket, text_len_buffer, 5); // recv on Windows
                uint32_t textlen = atoi(text_len_buffer);
                char text_buffer[textlen + 1];
                n = read(client_socket, text_buffer, textlen + 1); // recv on Windows
                
                MessagesQueue::Message new_message(nick, std::string(text_buffer));
                messages_queue->push_new_message(new_message);
            }
        }
    }

private:
    int client_socket;
    char *hostname;
    uint16_t port;
    char * nick;
    bool * user_is_typing;


    void read_user_input(int socket) {
        std::string input;
        while(true) {
            std::cin >> input;
            if (input == "m") {
                *user_is_typing = true;
                std::cout << "Please enter the message: ";
                std::cin >> input;
                uint32_t textlen = input.size();
                auto len = std::to_string(textlen).c_str();
                ssize_t n = write(socket, len, 4);
                auto text = input.c_str();
                /* Send message to the server */
                n = write(socket, text, strlen(text) + 1);

                if (n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
                *user_is_typing = false;
            }
        }
    }
};


#endif //MESSENGER_CLIENT_H
