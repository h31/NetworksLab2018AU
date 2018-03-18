#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include <strings.h>
#include <cstring>


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

    void write_to_socket(int socket) {
        int8_t nicklen = author.size();
        uint32_t textlen = text.size();
        char buffer[1];
        bzero(buffer, 1);
        buffer[0] = nicklen;
        ssize_t n = write(socket, buffer, 1); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        n = write(socket, author.c_str(), nicklen + 1); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        auto len = std::to_string(textlen).c_str();
        n = write(socket, len, 5); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
        n = write(socket, text.c_str(), text.size() + 1); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    static Message read_from_socket(int socket) {
        char buffer[1];
        bzero(buffer, 1);
        ssize_t n = read(socket, buffer, 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        int8_t nicklen = buffer[0];
        char nick_buffer[nicklen + 1];
        bzero(nick_buffer, nicklen + 1);
        n = read(socket, nick_buffer, nicklen + 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        std::string nick = std::string(nick_buffer);

        char text_len_buffer[5];
        bzero(text_len_buffer, 5);
        n = read(socket, text_len_buffer, 5); // recv on Windows
        uint32_t textlen = atoi(text_len_buffer);

        char text_buffer[textlen + 1];
        n = read(socket, text_buffer, textlen + 1); // recv on Windows
        std::string text = std::string(text_buffer);
        return Message(nick, text);
    }

private:
    std::string author;
    std::string text;
    tm * time_of_recieving;
};

class Messages {
public:
    void stop() {
        messages_mutex.lock();
        notified = true;
        new_messages.notify_all();
        messages_mutex.unlock();
    }
    
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

    void write_front_to_sockets(std::vector<int> * sockets) {
        Message new_message = poll_front();
        for (int client_socket : *sockets) {
            new_message.write_to_socket(client_socket);
        }
    }

    void write_front_to_socket(int socket) {
        Message new_message = poll_front();
        new_message.write_to_socket(socket);
    }

    bool read_and_push(int socket) {
        Message new_message = Message::read_from_socket(socket);
        if (new_message.get_text() == "#STOP") {
            return false;
        }
        push_new_message(new_message);
        return true;
    }
private:
    std::queue<Message> messages;
    std::mutex messages_mutex;
    std::condition_variable new_messages;
    bool notified = false;
};

#endif //MESSENGER_MESSAGE_H
