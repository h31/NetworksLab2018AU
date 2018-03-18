#ifndef MESSENGER_UI_H
#define MESSENGER_UI_H


#include <string>
#include <thread>
#include <iostream>
#include "../MessagesQueue.h"

class MessagesViewer {
public:
    MessagesViewer(MessagesQueue * messages_queue, bool * user_is_typing) {
        this->messages_queue = messages_queue;
        this->user_is_typing = user_is_typing;
        messages_viewer = std::thread(&MessagesViewer::view_new_messages, this);
    }
private:
    std::thread messages_viewer;
    MessagesQueue * messages_queue;
    bool * user_is_typing;

    void view_new_messages() {
        while(true) {
            while (!(*user_is_typing)) {
                auto new_message = messages_queue->poll_front();
                auto time = new_message.get_time();
                std::cout << "<" << time->tm_hour << ":";
                int min = time->tm_min;
                if (min < 10) {
                    std::cout << "0";
                }
                std::cout << time->tm_min << "> ";
                std::cout << "[" << new_message.get_author() << "] ";
                std::cout << new_message.get_text() << std::endl;
            }
        }
    }
};

#endif //MESSENGER_UI_H
