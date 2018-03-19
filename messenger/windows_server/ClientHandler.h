#ifndef MESSENGER_CLIENTHANDLER_H
#define MESSENGER_CLIENTHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <WinSock2.h>
#include "Messages.h"

class ClientHandler {
public:
    ClientHandler(Messages * messages, SOCKET client_socket):
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
    SOCKET client_socket;

    void handle_client() {
        bool result = true;
        while (result) {
            result = messages->read_and_push(client_socket);
        }
        closesocket(client_socket);
    }
};


#endif //MESSENGER_CLIENTHANDLER_H
