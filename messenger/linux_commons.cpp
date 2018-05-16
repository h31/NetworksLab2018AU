#include <netdb.h>
#include <netinet/in.h>
#include "commons.cpp"

#define SOCKET int

bool send_message(message &msg, SOCKET sock) {
    size_t len = 3 + msg.sender.length() + 6 + msg.message_text.length();
    char buffer[len + 1];
    snprintf(buffer, len + 1, "%03d%s%06d%s", msg.sender.length(), msg.sender.c_str(), msg.message_text.length(), msg.message_text.c_str());
    return send(sock, buffer, len, 0) == SO_ERROR;
}

message* recieve_message(SOCKET sock) {
    int res;
    char len_buffer[7];
    memset(len_buffer, 0, 7);
    res = recv(sock, len_buffer, 3, 0);
    if (res <= 0) {
        return nullptr;
    }
    size_t nickname_len = atoi(len_buffer);

    char nick_buffer[nickname_len + 1];
    memset(nick_buffer, 0, nickname_len + 1);
    res = recv(sock, nick_buffer, nickname_len, 0);
    if (res <= 0) {
        return nullptr;
    }

    memset(len_buffer, 0, 7);
    res = recv(sock, len_buffer, 6, 0);
    if (res <= 0) {
        return nullptr;
    }
    size_t msg_len = atoi(len_buffer);

    char msg_buffer[msg_len + 1];
    memset(msg_buffer, 0, msg_len + 1);
    res = recv(sock, msg_buffer, msg_len, 0);
    if (res <= 0) {
        return nullptr;
    }
    message* result = new message;
    result->sender = string(nick_buffer);
    result->message_text = string(msg_buffer);
    result->time = time(nullptr);
    return result;
}