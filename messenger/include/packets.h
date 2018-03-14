#pragma once

#include <ctime>
#include <string>

// base packet class
struct packet {};

// client packets
enum client_packet_type {
    LOGIN,
    MESSAGE,
    LOGOUT
};

struct client_packet : packet {
    client_packet_type type;
};

struct login_client_packet : client_packet { LOGIN } {
    std::string nickname;
};

struct message_client_packet : client_packet { MESSAGE } {
    std::string message;
};

struct logout_client_packet : client_packet { LOGOUT } {};

// server packets
struct server_packet : packet {
    std::time_t time_received;
    std::string sender_nickname;
    std::string message;
};

void write_packet(int, packet);
packet read_packet(int);