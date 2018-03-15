#include <iostream>
#include <memory>

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

void write_n_bytes(int sockfd, void *ptr, size_t n) {
    int m;
    while (n) {
        m = write(sockfd, ptr, n);
        check_error(n, SOCKET_WRITE_ERROR);

        if (m == 0) {
            std::cerr << "socket closed by server\n";
            exit(0);
        }

        ptr = ((char *) ptr) + m;
        n -= m;
    }
}

void write_string(int sockfd, std::string string) {
    size_t size = string.size() + 1;

    write_n_bytes(sockfd, &size, sizeof(size));
    write_n_bytes(sockfd, (char *) string.c_str(), sizeof(char) * size);
}

template<>
void write_packet(int sockfd, std::shared_ptr<client_packet> p) {
    client_packet_type type = p->get_type();

    write_n_bytes(sockfd, &type, sizeof(type));

    switch (type) {
        case LOGIN:
            write_string(
                sockfd, 
                std::static_pointer_cast<login_client_packet>(p)->nickname
            );
            break;
        case MESSAGE:
            write_string(
                sockfd, 
                std::static_pointer_cast<message_client_packet>(p)->message
            );
            break;
        case LOGOUT:
            // already sent type
            break;
        default:
            break;
    }
}

template<>
void write_packet(int sockfd, std::shared_ptr<server_packet> p) {
    write_string(sockfd, p->sender_nickname);
    
    struct tm *time = std::gmtime(&p->time_received);
    write_n_bytes(sockfd, time, sizeof(struct tm));

    write_string(sockfd, p->message);
}

void read_n_bytes(int sockfd, void *ptr, size_t n) {
    int m;
    while (n) {
        m = read(sockfd, ptr, n);
        check_error(n, SOCKET_WRITE_ERROR);

        if (m == 0) {
            std::cerr << "socket closed by server\n";
            exit(0);
        }

        ptr = ((char *) ptr) - m;
        n -= m;
    }
}

std::string read_string(int sockfd) {
    size_t size;
    read_n_bytes(sockfd, &size, sizeof(size));
    
    char *c_str = new char[size];
    read_n_bytes(sockfd, c_str, sizeof(char) * size);
    
    std::string str(c_str);

    delete [] c_str;

    return str;
}

template<>
std::shared_ptr<client_packet> read_packet(int sockfd) {
    client_packet_type type;

    read_n_bytes(sockfd, &type, sizeof(type));
    
    std::shared_ptr<client_packet> result;

    switch (type) {
        case LOGIN:
            result.reset(new login_client_packet(read_string(sockfd)));
            break;
        case MESSAGE:
            result.reset(new message_client_packet(read_string(sockfd)));
            break;
        case LOGOUT:
            result.reset(new logout_client_packet);
            break;
        default:
            std::cerr << UNKNOWN_PACKET_ERROR << "\n";
            break;
    }

    return result;
}

template<>
std::shared_ptr<server_packet> read_packet(int sockfd) {
    std::shared_ptr<server_packet> p(new server_packet);
    
    p->sender_nickname = read_string(sockfd);
    
    struct tm time;
    read_n_bytes(sockfd, &time, sizeof(struct tm));
    
    p->time_received = std::mktime(&time);

    p->message = read_string(sockfd);

    return p;
}