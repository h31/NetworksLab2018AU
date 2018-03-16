#pragma once

#include <ctime>
#include <string>
#include <type_traits>

// base packet class
struct packet {};

// client packets
enum client_packet_type {
    LOGIN,
    MESSAGE,
    LOGOUT
};

struct client_packet : packet {
    virtual client_packet_type get_type() = 0;
    virtual ~client_packet() = 0;
};

struct login_client_packet : client_packet {
    std::string nickname;

    client_packet_type get_type();
    login_client_packet(std::string);
    ~login_client_packet();
};

struct message_client_packet : client_packet {
    std::string message;

    client_packet_type get_type();
    message_client_packet(std::string);
    ~message_client_packet();
};

struct logout_client_packet : client_packet {
    client_packet_type get_type();
    ~logout_client_packet();
};

// server packets
struct server_packet : packet {
    std::time_t time_received;
    std::string sender_nickname;
    std::string message;

    server_packet();

    server_packet(std::time_t,std::string,std::string);
};

template<typename T>
void write_packet(SOCKET, std::shared_ptr<T>);

template<typename T>
std::shared_ptr<T> read_packet(SOCKET);

bool ready_to_read(SOCKET);