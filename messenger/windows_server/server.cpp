#define _WIN32_WINNT 0x501

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

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
                send(client, buffer, len, 0);
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
        if (recv(socket, &header[0], 24, 0) != 24) {
            break;
        }
        unsigned long long login_length = to_long(&header[8]);
        unsigned long long message_length = to_long(&header[16]);
        unsigned long long total_len = login_length + message_length + 24;
        char* buffer = new char[total_len];
        memcpy(buffer, &header[0], 24);
        if (recv(socket, &buffer[24], total_len - 24, 0) != total_len - 24) {
            break;
        }
        pthread_mutex_lock(&queue_mutex);
            buffers.push(buffer);
            sizes.push(total_len);
            buffer_size++;
        pthread_mutex_unlock(&queue_mutex);
    }
    pthread_mutex_lock(&clients_mutex);
            closesocket(socket);
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

    WSADATA wsaData;
    int err_code = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err_code != 0) {
        cout << "Error: WSAStartup failed. Error code: " << err_code << endl;
        return 2;
    }

    struct addrinfo* addr = NULL;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    err_code = getaddrinfo(NULL, argv[1], &hints, &addr);
    if (err_code != 0) {
        cout << "Error: getaddrinfo failed. Error code: " << err_code << endl;
        WSACleanup();
        return 3;
    }

    SOCKET listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        cout << "Error: socket creation failed." << endl;
        freeaddrinfo(addr);
        WSACleanup();
        return 4;
    }
    err_code = bind(listen_socket, addr->ai_addr, (int) addr->ai_addrlen);
    freeaddrinfo(addr);
    if (err_code == SOCKET_ERROR) {
        cout << "Error: unable to bind socket!" << endl;
        closesocket(listen_socket);
        WSACleanup();
        return 5;
    }

    err_code = listen(listen_socket, SOMAXCONN);
    if (err_code == SOCKET_ERROR) {
        cout << "Error: unable to start listening socket" << endl;
        closesocket(listen_socket);
        WSACleanup();
        return 6;
    }
    pthread_t broadcast_thread;
    err_code = pthread_create(&broadcast_thread, NULL, broadcaster, (void*) 0);
    if (err_code < 0) {
        cout << "Error: failed to start broadcast-thread" << endl;
        closesocket(listen_socket);
        WSACleanup();
        return 7;
    }
    SOCKET client_socket;
    while (true) {
        client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
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
