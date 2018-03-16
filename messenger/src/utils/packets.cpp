#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>

#include <network.h>
#include <utils/errors.h>
#include <utils/packets.h>

std::mutex time_io_mutex;

client_packet::~client_packet() = default;

login_client_packet::login_client_packet(std::string _nickname) : nickname(_nickname) {}
login_client_packet::~login_client_packet() = default;

client_packet_type login_client_packet::get_type() {
    return LOGIN;
}

message_client_packet::message_client_packet(std::string _message) : message(_message) {}
message_client_packet::~message_client_packet() = default;

client_packet_type message_client_packet::get_type() {
    return MESSAGE;
}

logout_client_packet::~logout_client_packet() = default;

client_packet_type logout_client_packet::get_type() {
    return LOGOUT;
}

server_packet::server_packet() = default;

server_packet::server_packet(
    std::time_t _time_received,
    std::string _sender_nickname,
    std::string _message
) : time_received(_time_received), sender_nickname(_sender_nickname), message(_message) {}

void write_n_bytes(SOCKET sockfd, void *ptr, int n) {
    int m;
    while (n > 0) {
        m = send(sockfd, (char *) ptr, n, 0);
        check_error(n, SOCKET_WRITE_ERROR);

        if (m == 0) {
            throw std::string("socket closed by remote side");
        }

        ptr = ((char *) ptr) + m;
        n -= m;
    }
}

void write_string(SOCKET sockfd, std::string string) {
    size_t size = string.size() + 1;

    write_n_bytes(sockfd, &size, sizeof(size));
    write_n_bytes(sockfd, (char *) string.c_str(), sizeof(char) * size);
}

template<>
void write_packet(SOCKET sockfd, std::shared_ptr<client_packet> p) {
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
void write_packet(SOCKET sockfd, std::shared_ptr<server_packet> p) {
    write_string(sockfd, p->sender_nickname);

    std::string time_string;
    std::ostringstream time_string_ostream(time_string);

    time_io_mutex.lock();
    time_string_ostream << std::put_time(std::gmtime(&p->time_received), "%FT%TZ");
    time_io_mutex.unlock();

    time_string_ostream.flush();

    write_string(sockfd, time_string);

    write_string(sockfd, p->message);
}

void read_n_bytes(SOCKET sockfd, void *ptr, int n) {
    int m;
    while (n > 0) {
        m = recv(sockfd, (char *) ptr, n, 0);
        check_error(n, SOCKET_READ_ERROR);

        if (m == 0) {
            throw std::string("socket closed by remote side");
        }

        ptr = ((char *) ptr) - m;
        n -= m;
    }
}

std::string read_string(SOCKET sockfd) {
    size_t size;
    read_n_bytes(sockfd, &size, sizeof(size));
    
    auto c_str = new char[size];
    read_n_bytes(sockfd, c_str, sizeof(char) * size);

    std::string str(c_str);

    delete [] c_str;

    return str;
}

template<>
std::shared_ptr<client_packet> read_packet(SOCKET sockfd) {
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
std::shared_ptr<server_packet> read_packet(SOCKET sockfd) {
    std::shared_ptr<server_packet> p(new server_packet);
    
    p->sender_nickname = read_string(sockfd);

    std::istringstream time_string_istream(read_string(sockfd));

    time_io_mutex.lock();
    std::tm time_struct;
    time_string_istream >> std::get_time(&time_struct, "%FT%TZ");
    p->time_received = mktime(&time_struct);
    time_io_mutex.unlock();

    p->message = read_string(sockfd);

    return p;
}

const int POLL_TIMEOUT = 10;

bool ready_to_read(SOCKET sockfd) {
    POLLFD poll_settings {};
    poll_settings.fd = sockfd;
    poll_settings.events = POLLIN;
    
    int poll_result = poll(&poll_settings, 1, POLL_TIMEOUT);
    check_error(poll_result, POLL_ERROR);

    return poll_result > 0;
}