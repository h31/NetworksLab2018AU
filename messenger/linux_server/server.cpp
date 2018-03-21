#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <queue>
#include <vector>

#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;

using namespace std;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int buffer_size = 0;
queue<char*> buffers;
queue<long> sizes;


pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
vector<SOCKET> clients;

void* broadcaster(void *arg) {
    while (true) {
        while (buffer_size == 0);
        char* buffer;
        long len;
        pthread_mutex_lock(&queue_mutex);
            buffer = buffers.front();
            buffers.pop();
            len = sizes.front();
            sizes.pop();
            buffer_size--;
        pthread_mutex_unlock(&queue_mutex);

        pthread_mutex_lock(&clients_mutex);
            for (SOCKET client : clients) {
                write(client, buffer, len);
            }
        pthread_mutex_unlock(&clients_mutex);
        delete buffer;
    }
    return (void*) 0;
}

unsigned long long to_long(char* arr) {
    unsigned long long res = *((unsigned long long*) arr);
    return res;
}

void* connection_listener(void *arg) {
    char* header = new char[24];
    SOCKET socket = (SOCKET) arg;
    while (true) {
        if (read(socket, &header[0], 24) != 24) {
            break;
        }
        unsigned long long login_length = to_long(&header[8]);
        unsigned long long message_length = to_long(&header[16]);
        unsigned long long total_len = login_length + message_length + 24;
        char* buffer = new char[total_len];
        memcpy(buffer, &header[0], 24);
        if (read(socket, &buffer[24], total_len - 24) != total_len - 24) {
            break;
        }
        pthread_mutex_lock(&queue_mutex);
            buffers.push(buffer);
            sizes.push(total_len);
            buffer_size++;
        pthread_mutex_unlock(&queue_mutex);
    }
    pthread_mutex_lock(&clients_mutex);
            close(socket);
            for (int i = 0; i < clients.size(); i++) {
                if (clients[i] == socket) {
                    clients.erase(clients.begin() + i);
                    break;
                }
            }
    pthread_mutex_unlock(&clients_mutex);
    return (void*) 0;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Error: wrong arguments formate. Expected: \"host\"" << endl;
        return 1;
    }
    int port_number = atoi(argv[1]);
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) {
        cout << "Error: socket creation failed." << endl;
        return 2;
    }
    struct sockaddr_in addr;
    memset((char *) &addr, sizeof(addr), 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_number);
    if (bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        cout << "Error: unable to bind socket!" << endl;
        close(listen_socket);
        return 3;
    }

    pthread_t broadcast_thread;
    int err_code = pthread_create(&broadcast_thread, NULL, broadcaster, (void*) 0);
    if (err_code < 0) {
        cout << "Error: failed to start broadcast-thread" << endl;
        close(listen_socket);
        return 7;
    }

   
    listen(listen_socket, 5);
    struct sockaddr_in cli_addr;
    unsigned int cli_len = sizeof(cli_addr);
    while (true) {
        SOCKET client_socket = accept(listen_socket, (struct sockaddr *) &cli_addr, &cli_len);
        if (client_socket < 0) {
            cout << "Warning: connecting acceptance failed" << endl;
        } else {
            pthread_mutex_lock(&clients_mutex);
                clients.push_back(client_socket);
            pthread_mutex_unlock(&clients_mutex);
            pthread_t client_thread;
            err_code = pthread_create(
                &client_thread, NULL, connection_listener, (void*) client_socket);
            if (err_code < 0) {
                cout << "Warning: listener-thread creating failed" << endl;
            }
        }
    }
}
