#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H


#include <algorithm>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <time.h>


class MessagesQueue {
public:
    class Message;
    Message poll_front() {
        std::unique_lock<std::mutex> lock(messages_mutex);
        while (!notified) {
            new_messages.wait(lock);
        }
        Message front_message = messages.front();
        messages.pop();
        notified = false;
        return front_message;
    }

    void push_new_message(const Message &message) {
        messages_mutex.lock();
        messages.push(message);
        notified = true;
        new_messages.notify_all();
        messages_mutex.unlock();
    }

class Message {
public:
    Message(std::string author, std::string text) : author(std::move(author)), text(std::move(text)) {
        time_t theTime = time(nullptr);
        time_of_recieving = localtime(&theTime);
    }

    std::string get_author() {
        return author;
    }

    std::string get_text() {
        return text;
    }

    tm * get_time() {
        return time_of_recieving;
    }
private:
    std::string author;
    std::string text;
    tm * time_of_recieving;
};

private:
    std::queue<Message> messages;
    std::mutex messages_mutex;
    std::condition_variable new_messages;
    bool notified = false;
};


#endif //MESSENGER_MESSAGE_H
