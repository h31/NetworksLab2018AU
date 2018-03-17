//
// Created by kate on 16.03.18.
//

#include <iostream>
#include <getopt.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "../common/client_utils.h"

bool is_exited_client = false;
bool is_mode_read = false;

#define SET_THIS_FILE_NAME()
static const char *const THIS_FILE_NAME = \
        strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__;


void print_error(int line, const std::string &mess) {
    std::cerr << THIS_FILE_NAME << "[" << line << "] ERROR: " << mess
              << std::endl;
}

int send_login_to_server(int id_socket, const clien &data_client) {
    return sender(id_socket, data_client, true,
                  std::bind(print_error, __LINE__, std::placeholders::_1));
}

int send_mess_to_server(int id_socket, const clien &data_client) {
    return sender(id_socket, data_client, false,
                  std::bind(print_error, __LINE__, std::placeholders::_1));
}


void *reader_message(void *id_server) {

    while (!is_exited_client) {
        uint16_t id_socket = *(uint16_t *) id_server;
        clien date_client;
        int n = reader(id_socket, date_client,
                       std::bind(print_error, __LINE__, std::placeholders::_1));
        if (n == 0) {
//            std::cout << "wait mode reader_message " << std::endl;
            while (is_mode_read);
            std::cout << date_client << std::endl;

        }
    }
}


void set_line(clien &data_client, char *mess) {
    std::cout << "get mess " << mess << " size=" << strlen(mess) << std::endl;
    data_client.mess.size = strlen(mess);
    data_client.mess.cap = strlen(mess) + 1;
    data_client.mess.mess = new char[sizeof(char) * data_client.mess.cap];
    std::cout << " set val to client_data\n";
    memcpy(data_client.mess.mess, mess, data_client.mess.size);
    data_client.mess.mess[data_client.mess.size] = '\0';

    get_current_time(data_client.time);
    std::cout << "send: " << data_client << std::endl;
}

void get_argv(int argc, char **argv, char **address_server,
              uint16_t &port_server, char **nick_name) {

    int res;
    while ((res = getopt(argc, argv, "s:p:n:")) != -1) {
        switch (res) {
            case 's': {
                size_t len = strlen(optarg);
                *address_server = static_cast<char *>(malloc(
                        sizeof(char) * (len + 1)));
                strncpy(*address_server, optarg, len);
                *(address_server + len) = '\0';
                break;
            }
            case 'p': {
                port_server = (uint16_t) (atoi(optarg));
                break;
            }
            case 'n': {
                size_t len = strlen(optarg);
                *nick_name = static_cast<char *>(malloc(
                        sizeof(char) * (len + 1)));
                strncpy(*nick_name, optarg, len);
                *(nick_name + len) = '\0';
                break;
            }
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {

    using namespace std;
    if (argc < 3) {
//        std::cout << " " << std::endl;
    }

    char *address_server;
    uint16_t port_server;
    char *nick_name;

    get_argv(argc, argv, &address_server, port_server, &nick_name);

    clien data_client(nick_name);
    std::cout << " get client nick " << nick_name << std::endl;
    std::cout << data_client << std::endl;

    /* Create a socket point */
    std::cout << "create conntection " << std::endl;
    int id_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (id_socket < 0) {
        print_error(__LINE__, " opening socket");
        exit(1);
    }
    struct hostent *server = gethostbyname(address_server);

    if (server == nullptr) {
        print_error(__LINE__, "  no such host");
        exit(0);
    }

    struct sockaddr_in serv_addr{};
    memset((char *) &address_server, 0, sizeof(address_server));

    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
          (size_t) server->h_length);
    serv_addr.sin_port = htons(port_server);

    /* Now connect to the server */
    if (connect(id_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <
        0) {
        print_error(__LINE__, " connecting");
        exit(1);
    }

    std::cout << "create connection to server " << id_socket << std::endl;
    int res = send_login_to_server(id_socket, data_client.nick);
    if (res != 0) {
        exit(1);
    }
    pthread_t updates_thread;
    int code = pthread_create(&updates_thread, NULL, reader_message,
                              &id_socket);
    if (code < 0) {
        print_error(__LINE__, "on creating reader thread");
        exit(1);
    }

    std::string specification = ":m - write message\n:q - exit";

    std::cout << specification << std::endl;

    char line[256];
    while (!is_exited_client) {
        std::cin.getline(line, 256);
        std::string m(line);
        if (m.find(":m") == 0) {
            is_mode_read = true;
            set_line(data_client, line + 2);
            is_mode_read = false;
            send_mess_to_server(id_socket, data_client);

        } else {
            if (m.find(":q") == 0) {
                is_exited_client = true;
                data_client.set_empty();
                send_login_to_server(id_socket, data_client);
                continue;
            }
        }
    }

    if (close(id_socket) < 0) {
        print_error(__LINE__, "closed socket");
        return 1;
    }

    return 0;
}