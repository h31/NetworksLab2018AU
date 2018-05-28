#ifndef MESSENGER_MESSAGESENDER_H
#define MESSENGER_MESSAGESENDER_H


#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "Messages.h"

class MessageSender {
public:
    MessageSender(Messages * messages,
                  std::vector<int> * clients_sockets) :
            messages(messages),
            connected_clients_sockets(clients_sockets) {
        message_sender = std::thread(&MessageSender::handle_new_messages, this);
    }

    void stop() {
        message_sender.join();
    }

private:
    Messages * messages;
    std::vector<int> * connected_clients_sockets;
    std::thread message_sender;
    
    void handle_new_messages() {
        while (true) {
            messages->write_front_to_sockets(connected_clients_sockets);
        }
    }
};


#endif //MESSENGER_MESSAGESENDER_H
