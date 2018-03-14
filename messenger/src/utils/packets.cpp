#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <utils/errors.h>
#include <utils/packets.h>

client_packet::~client_packet() {}

login_client_packet::login_client_packet(std::string _nickname) : nickname(_nickname) {}
login_client_packet::~login_client_packet() {}
client_packet_type login_client_packet::get_type() {
    return LOGIN;
}

message_client_packet::message_client_packet(std::string _message) : message(_message) {}
message_client_packet::~message_client_packet() {}
client_packet_type message_client_packet::get_type() {
    return MESSAGE;
}

logout_client_packet::~logout_client_packet() {}
client_packet_type logout_client_packet::get_type() {
    return LOGOUT;
}


server_packet::server_packet() = default;

server_packet::server_packet(
    std::time_t _time_received,
    std::string _sender_nickname,
    std::string _message
) : time_received(_time_received), sender_nickname(_sender_nickname), message(_message) {}


void write_string(int sockfd, std::string string) {
    int n;

    size_t size = string.size() + 1;

    n = write(sockfd, &size, sizeof(size));
    check_error(n, SOCKET_WRITE_ERROR);

    n = write(sockfd, string.c_str(), sizeof(char) * size);
    check_error(n, SOCKET_WRITE_ERROR);
}

template<>
void write_packet(int sockfd, client_packet *p) {
    int n;

    client_packet_type type = p->get_type();

    n = write(sockfd, &type, sizeof(type));
    check_error(n, SOCKET_WRITE_ERROR);

    switch (type) {
        case LOGIN:
            write_string(sockfd, static_cast<login_client_packet*>(p)->nickname);
            break;
        case MESSAGE:
            write_string(sockfd, static_cast<message_client_packet*>(p)->message);
            break;
        case LOGOUT:
            // already sent type
            break;
        default:
            break;
    }
}

template<>
void write_packet(int sockfd, server_packet *p) {
    write_string(sockfd, p->sender_nickname);
    
    struct tm *time = std::gmtime(&p->time_received);
    write(sockfd, time, sizeof(struct tm));

    write_string(sockfd, p->message);
}

std::string read_string(int sockfd) {
    int n;

    size_t size;

    n = read(sockfd, &size, sizeof(size));
    check_error(n, SOCKET_READ_ERROR);

    char *c_str = new char[size];

    n = read(sockfd, c_str, sizeof(char) * size);
    check_error(n, SOCKET_WRITE_ERROR);

    std::string str(c_str);

    delete [] c_str;

    return str;
}

template<>
client_packet *read_packet(int sockfd) {
    int n;

    client_packet_type type;

    n = read(sockfd, &type, sizeof(type));
    check_error(n, SOCKET_WRITE_ERROR);

    switch (type) {
        case LOGIN:
            return new login_client_packet(read_string(sockfd));
        case MESSAGE:
            return new message_client_packet(read_string(sockfd));
        case LOGOUT:
            return new logout_client_packet;
        default:
            return NULL;
    }
}

template<>
server_packet *read_packet(int sockfd) {
    server_packet *p = new server_packet;

    p->sender_nickname = read_string(sockfd);
    
    struct tm time;
    int n = read(sockfd, &time, sizeof(struct tm));
    check_error(n, SOCKET_READ_ERROR);
    p->time_received = std::mktime(&time);

    p->message = read_string(sockfd);

    return p;
}