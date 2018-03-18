#ifndef MESSENGER_CLIENTHANDLER_H
#define MESSENGER_CLIENTHANDLER_H

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <strings.h>
#include "../MessagesQueue.h"

class ClientHandler {
public:
    ClientHandler(MessagesQueue * messages, int client_socket):
            messages(messages),
            client_socket(client_socket) {
        char buffer[256];
        ssize_t n;
        bzero(buffer, 256);
        n = read(client_socket, buffer, 255);
        nick = std::string(buffer);
    }

    void run() {
        client_handler = new std::thread(&ClientHandler::handle_client, this);
    }

    ClientHandler(const ClientHandler & another) {
        messages = another.messages;
        client_handler = another.client_handler;
        client_socket = another.client_socket;
    }

    void stop() {
        (*client_handler).join();
        delete client_handler;
    }
private:
    MessagesQueue * messages;
    std::thread * client_handler;
    int client_socket;
    std::string nick;
    size_t buffer_size = 1024;

    void handle_client() {
        char buffer[5];

        while (true) {
            /* If connection is established then start communicating */
            bzero(buffer, buffer_size);
            ssize_t n = read(client_socket, buffer, 5); // recv on Windows
            uint32_t textlen = atoi(buffer);
            char text_buffer[textlen + 1];
            n = read(client_socket, text_buffer, textlen + 1); // recv on Windows
            MessagesQueue::Message new_message(nick, std::string(text_buffer));
            messages->push_new_message(new_message);
            if (n < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }
            printf("Here is the message: %s\n", text_buffer);
        }
    }
};


#endif //MESSENGER_CLIENTHANDLER_H
