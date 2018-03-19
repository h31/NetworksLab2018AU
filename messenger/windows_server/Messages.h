#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <queue>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include <cstring>
#include <WinSock2.h>


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

    void write_to_socket(SOCKET socket) {
        int8_t nicklen = author.size();
        uint32_t textlen = text.size();
        char buffer[1];
        buffer[0] = nicklen;
        int n = send(socket, buffer, 1, 0); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        n = send(socket, author.c_str(), nicklen + 1, 0); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
		textlen = htonl(textlen);
        auto len = (char*)(&textlen);
        n = send(socket, len, 5, 0); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
        n = send(socket, text.c_str(), text.size() + 1, 0); // send on Windows

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    static Message read_from_socket(int socket) {
        char buffer[1];
        int n = recv(socket, buffer, 1, 0);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        int nicklen = buffer[0];
        char *nick_buffer = new char[nicklen + 1];
        n = recv(socket, nick_buffer, nicklen + 1, 0);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        std::string nick = std::string(nick_buffer);
		delete[] nick_buffer;
        uint32_t textlen;
        n = recv(socket, (char*)&textlen, 5, 0); // recv on Windows
		textlen = ntohl(textlen);
        char * text_buffer = new char[textlen + 1];
        n = recv(socket, text_buffer, textlen + 1, 0); // recv on Windows
        std::string text = std::string(text_buffer);
		delete[] text_buffer;
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
