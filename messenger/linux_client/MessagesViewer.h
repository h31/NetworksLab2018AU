#ifndef MESSENGER_UI_H
#define MESSENGER_UI_H


#include <string>
#include <thread>
#include <iostream>
#include "../Messages.h"

class MessagesViewer {
public:
    MessagesViewer(Messages * messages_queue, bool * user_is_typing) {
        this->messages_queue = messages_queue;
        this->user_is_typing = user_is_typing;
        messages_viewer = std::thread(&MessagesViewer::view_new_messages, this);
    }
    
    void stop() {
        stopped = true;
        messages_queue->stop();
        messages_viewer.join();
    }
private:
    std::thread messages_viewer;
    Messages * messages_queue;
    bool * user_is_typing;
    bool stopped = false;

    void view_new_messages() {
        while(!stopped) {
            while (!(*user_is_typing) && !stopped) {
                auto new_message = messages_queue->poll_front();
                if (stopped) return;
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
