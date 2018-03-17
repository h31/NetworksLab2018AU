//
// Created by kate on 17.03.18.
//

#ifndef SERVER_LINUX_CLIENT_UTILS_H
#define SERVER_LINUX_CLIENT_UTILS_H

#include <cstring>
#include <utility>
#include <ostream>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>
#include <unistd.h>

static const int TIME_SIZE = 80;

struct message_data {

    explicit message_data(size_t size) : size(size), cap(size + 1) {}

    message_data() : size(0), cap(0) {}

    size_t size;
    size_t cap;
    char *mess = nullptr;
};

struct clien {
    char *nick = nullptr;
    char time[TIME_SIZE]{};
    message_data mess;

    explicit clien(char *nick) : nick(nick), time() {}

    clien() = default;

    void set_empty() {
        delete[] nick;
        delete[] mess.mess;
        nick = nullptr;
        mess.mess = nullptr;
        mess.size = 0;
        mess.cap = 0;
    }

    char *create_mess() const {
        size_t size_nick = get_len_login();
        size_t size_mess = mess.size;
        auto *m = new char[size_nick + TIME_SIZE + size_mess + 1];


        memcpy(m, nick, size_nick);
        memcpy(m + size_nick, time, TIME_SIZE);
        memcpy(m + size_nick + TIME_SIZE, mess.mess, size_mess);
        m[size_nick + TIME_SIZE + size_mess] = '\0';

        return m;
    }

    void set_nick(char *message, size_t size) {
        nick = new char[size];
        strncpy(nick, message, size);
    }

    void set_mess(char *message, size_t size) {
        mess.mess = new char[size];
        mess.size = size;
        mess.cap = size + 1;
        strncpy(mess.mess, message, size);
    }

    void set_time(char *message, size_t size = TIME_SIZE) {
        strncpy(time, message, size);
    }

    size_t get_len_login() const {
        return nick != nullptr ? strlen(nick) : 0;
    }

    size_t get_len_mess() const {
        return mess.size;
    }

    friend std::ostream &operator<<(std::ostream &os, const clien &clien1) {
        //<%s> [%s] %s
        os << " <" << clien1.time << "> ";
        if (clien1.get_len_login() > 0) {
            os << " [" << clien1.nick << "] ";
        }

        if (clien1.get_len_mess() > 0) {
            os << clien1.mess.mess;
        }
        return os;
    }
};


static void get_current_time(char time_res[TIME_SIZE]) {
    time_t rawtime;
    time(&rawtime);
    strftime(time_res, TIME_SIZE, "%d-%m-%Y %I:%M:%S", localtime(&rawtime));
}

int sender(int id_socket, const clien &data_client, bool only_nick,
           const std::function<void(const std::string &)> &print_error) {
    size_t size_nick = data_client.get_len_login();

    size_t size_time = 0;
    size_t size_mess = 0;

    if (!only_nick) {
        size_time = TIME_SIZE;
        size_mess = data_client.get_len_mess();
    }

    ssize_t n_nick = write(id_socket, &size_nick, 4);
    ssize_t n_time = write(id_socket, &size_time, 4);
    ssize_t n_mess = write(id_socket, &size_mess, 4);

    if (n_nick != 4 || n_mess != 4 || n_time != 4) {
        print_error(" write size to socket");
        return -1;
    }


    ssize_t n = 0;
    if (only_nick) {
        if (size_nick != 0) {
            n = write(id_socket, data_client.nick, size_nick);
        }
    } else {
        char *mess = data_client.create_mess();
        n = write(id_socket, mess, size_nick + size_time + size_mess);

    }
    if (n < 0) {
        print_error(" write message to socket");
        return -1;
    }
    return 0;
}

int reader(int id_socket, clien &clien_data,
           const std::function<void(const std::string &)> &print_error) {
    size_t nick_size = 0,
            mess_size = 0,
            time_size = 0;

    ssize_t n_nick = read(id_socket, &nick_size, 4);
    ssize_t n_time = read(id_socket, &time_size, 4);
    ssize_t n_mess = read(id_socket, &mess_size, 4);
    if (n_nick != 4 || n_time != 4 || n_mess != 4) {
        print_error(" read mess_size from socket");
        return -1;
    }
    if (nick_size == 0 && time_size == 0 && mess_size == 0) {
        // exit mess
        return 1;
    }

    size_t size = nick_size + time_size + mess_size + 1;

    auto *message = new char[size + 1];
    uint32_t read_size = 0;
    ssize_t n = 0;
    while (read_size < size - 1) {
        n = read(id_socket, message + read_size, size - read_size);
        if (n <= 0) {
            print_error("read from socket ");
            return -1;
        }
        read_size += n;
    }


    message[nick_size + time_size + mess_size] = '\0';

    clien_data.set_nick(message, nick_size);
    if (time_size != 0 && mess_size != 0) {
        clien_data.set_time(message + nick_size, time_size);
        clien_data.set_mess(message + nick_size + time_size, mess_size);
    }
    return 0;
}

#endif //SERVER_LINUX_CLIENT_UTILS_H
