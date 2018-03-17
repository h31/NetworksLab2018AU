//
// Created by kate on 16.03.18.
//
#include <iostream>
#include <netdb.h>
#include <memory.h>
#include <pthread.h>
#include <getopt.h>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <algorithm>

#include "../common/client_utils.h"

pthread_mutex_t mutex_client = PTHREAD_MUTEX_INITIALIZER;

std::vector<int> socket_idx;
int count_client = 0;

#define SET_THIS_FILE_NAME()
static const char *const THIS_FILE_NAME = \
        strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__;

void print_error(int line, const std::string &mess) {
    char time[80];
    get_current_time(time);
    std::cerr << time << ":" << THIS_FILE_NAME << " [" << line << "] ERROR: "
              << mess << std::endl;
}

void add_new_client(int id_socket) {
    std::cout << "add new client " << std::endl;
    pthread_mutex_lock(&mutex_client);
    count_client++;
    socket_idx.push_back(id_socket);
    std::cout << " add client with id " << id_socket << std::endl;
    pthread_mutex_unlock(&mutex_client);
}

void remove_client(int id_socket) {
    pthread_mutex_lock(&mutex_client);
    socket_idx.erase(
            std::remove(socket_idx.begin(), socket_idx.end(), id_socket),
            socket_idx.end());
    pthread_mutex_unlock(&mutex_client);
}


int write_message(const clien &client_data) {
    using namespace std::placeholders;
    pthread_mutex_lock(&mutex_client);
    for (auto item : socket_idx) {
        int n = sender(item, client_data, false,
                       std::bind(print_error, __LINE__, _1));
        if (n < 0) {
            print_error(__LINE__, "write size to socket " + item);
            continue;
        }
    }
    pthread_mutex_unlock(&mutex_client);


    return 0;
}

int read_message(int id_socket, clien &clien_data) {
    return reader(id_socket, clien_data,
                  std::bind(print_error, __LINE__, std::placeholders::_1));
}

void *handle_client(void *client_socket) {
    std::cout << "run handle_client " << std::endl;

    uint16_t id_socket = *(uint16_t *) client_socket;

    add_new_client(id_socket);

    clien clien_data;
    std::cout << " wait nick name " << std::endl;
    if (read_message(id_socket, clien_data) < 0) {
        print_error(__LINE__, " can not read login client; connection "
                "failed");
        return NULL;
    }
    std::cout << "get client " << clien_data << std::endl;
    while (true) {
        int res = read_message(id_socket, clien_data);
        if (res < 0) {
            print_error(__LINE__, "not read message from client");
            continue;
        }
        if (res == 1) {
            std::cout << "exit client \n";
            // exit mess
            remove_client(id_socket);
            std::cout << " return NULL \n";
            return NULL;
        } else {
            std::cout << clien_data << std::endl;
            if (write_message(clien_data) != 0) {
                std::cout << " not write mess \n";
            }
        }
    }

}

int run_server(int64_t port) {


    int id_socket;
    struct sockaddr_in address;

    if ((id_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        print_error(__LINE__, "can not open socket ");
        return -1;
    }
    memset((char *) &address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(id_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        print_error(__LINE__, "can not bind socket ");
        return -1;
    }

    listen(id_socket, 5);
    struct sockaddr_in cli_addr;
    unsigned int clilen = sizeof(cli_addr);


    while (true) {
        int client_socket = accept(id_socket, (struct sockaddr *) &cli_addr,
                                   &clilen);

        if (client_socket < 0) {
            print_error(__LINE__, "can not on accept " + id_socket);
            continue;
        }

//        std::cout << " accept new client " << client_socket << std::endl;
        pthread_t client_thread;
        int id_thread = pthread_create(&client_thread, NULL, handle_client,
                                       &client_socket);

        if (id_thread < 0) {
            print_error(__LINE__, "can not start  thread for client");
        }
    }


}


int main(int argc, char *argv[]) {

    using namespace std;
    if (argc < 2) {
        cout << " args: -p port " << endl;
        return 1;
    }

    int port;
    size_t server_port = 0;
    while ((port = getopt(argc, argv, "p:")) != -1) {
        if (port == 'p') {
            server_port = static_cast<size_t>(atoi(optarg));
        }
    }

    return run_server(server_port);
}