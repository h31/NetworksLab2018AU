#ifndef MESSENGER_MESSAGESENDER_H
#define MESSENGER_MESSAGESENDER_H


#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <queue>
#include <condition_variable>
#include <thread>
#include <strings.h>
#include "../MessagesQueue.h"

class MessageSender {
public:
    MessageSender(MessagesQueue * messages,
                  std::vector<int> * clients_sockets) :
            messages(messages),
            connected_clients_sockets(clients_sockets) {
        message_sender = std::thread(&MessageSender::handle_new_messages, this);
    }

    void stop() {
        message_sender.join();
    }

private:
    MessagesQueue * messages;
    std::vector<int> * connected_clients_sockets;
    std::thread message_sender;
    
    void handle_new_messages() {
        while (true) {
            MessagesQueue::Message new_message = messages->poll_front();
            std::string nick = new_message.get_author();
            int8_t nicklen = new_message.get_author().size();
            uint32_t textlen = new_message.get_text().size();
            char buffer[1];
            bzero(buffer, 1);
            buffer[0] = nicklen;
            for (int client_socket : *connected_clients_sockets) {
                ssize_t n = write(client_socket, buffer, 1); // send on Windows

                if (n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
            }
            
            char nick_buffer[nicklen];
            for (unsigned int charno = 0; charno < nicklen; charno++) {
                nick_buffer[charno] = nick[charno];
            }
            for (int client_socket : *connected_clients_sockets) {
                ssize_t n = write(client_socket, nick_buffer, nicklen); // send on Windows

                if (n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
            }
            
            auto len = std::to_string(textlen).c_str();
            for (int client_socket : *connected_clients_sockets) {
                ssize_t n = write(client_socket, len, 4); // send on Windows

                if (n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
            }
            
            std::string text = new_message.get_text();
            
            for (int client_socket : *connected_clients_sockets) {
                ssize_t n = write(client_socket, text.c_str(), text.size()); // send on Windows

                if (n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
            }
        }
    }
};


#endif //MESSENGER_MESSAGESENDER_H
