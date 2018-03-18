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
#include "../Messages.h"

class ClientHandler {
public:
    ClientHandler(Messages * messages, int client_socket):
            messages(messages),
            client_socket(client_socket) {
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
    Messages * messages;
    std::thread * client_handler;
    int client_socket;

    void handle_client() {
        bool result = true;
        while (result) {
            result = messages->read_and_push(client_socket);
        }
        close(client_socket);
    }
};


#endif //MESSENGER_CLIENTHANDLER_H
