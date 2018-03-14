#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT "5001"

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

SOCKET *client_sockets;
int clients_array_size, clients_count;

void send_message_to_everyone(char* login, char* message, char* time, int socket_index) {
    uint8_t login_length = (uint8_t) strlen(login);
    uint8_t message_length = (uint8_t) strlen(message);
    char buffer[518];
    buffer[0] = login_length;
    buffer[1] = message_length;
    strncpy(buffer + 2, time, 6);
    strcpy(buffer + 8, login);
    strcpy(buffer + 8 + login_length, message);
    pthread_mutex_lock(&clients_mutex);
    printf("Sending message from %d\n", socket_index);
    for (int i = 0; i < clients_count; i++) {
        if (client_sockets[i] == INVALID_SOCKET) {
            continue;
        }
        int n = send(client_sockets[i], buffer, strlen(buffer), 0);
        if (n < 0) {
            printf("Could not write the message to socket with index %d\n", i);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int register_client(SOCKET socket) {
    int result_index = -1;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < clients_count; i++) {
        if (client_sockets[i] == INVALID_SOCKET) {
            client_sockets[i] = socket;
            result_index = i;
            break;
        }
    }
    if (result_index == -1) {
        if (clients_count == clients_array_size) {
            clients_array_size *= 2;
            client_sockets = realloc(client_sockets, sizeof(SOCKET) * clients_array_size);
        }
        result_index = clients_count;
        client_sockets[result_index] = socket;
        clients_count += 1;
    }
    pthread_mutex_unlock(&clients_mutex);
    printf("Client with socket index %d was registered\n", result_index);
    return result_index;
}

void deregister_client(int socket_index) {
    pthread_mutex_lock(&clients_mutex);
    client_sockets[socket_index] = INVALID_SOCKET;
    printf("Client with socket index %d was deregistered\n", socket_index);
    pthread_mutex_unlock(&clients_mutex);
}

void inner_client_handling(int socket_index) {
    SOCKET socket = client_sockets[socket_index];
    /* Reading login */
    char login[256];
    memset(login, 0, 256);
    if (recv(socket, login, 1, 0) != 1) {
        printf("Connection with %d will be closed due to login failure\n", socket_index);
        return;
    }
    uint8_t login_length = (uint8_t) login[0];
    login[0] = 0;
    if (recv(socket, login, login_length, 0) != login_length) {
        printf("Connection with %d will be closed due to login failure\n", socket_index);
        return;
    }
    /* If login is read then start communicating */
    printf("Socket id %d is %s\n", socket_index, login);
    char buffer[256];
    while (1) {
        memset(buffer, 0, 256);
        if (recv(socket, buffer, 1, 0) != 1) {
            printf("Connection with %d will be closed due to recv() error\n", socket_index);
            return;
        }
        uint8_t buffer_length = (uint8_t) buffer[0];
        buffer[0] = 0;
        if (recv(socket, buffer, buffer_length, 0) != buffer_length) {
            printf("Connection with %d will be closed due to recv() error\n", socket_index);
            return;
        }
        time_t rawtime;
        struct tm* timeinfo;
        rawtime = time(NULL);
        timeinfo = localtime(&rawtime);
        char time_buffer[7];
        time_buffer[6] = 0;
        strftime(time_buffer, 7, "%H%M%S", timeinfo);
        printf("time %s\n", time_buffer);
        printf("Received the message from %d: %s\n", socket_index, buffer);
        send_message_to_everyone(login, buffer, time_buffer, socket_index);
    }
}

void *client_handling_routine(void *arg) {
    int socket_index = (int) arg;
    inner_client_handling(socket_index);
    closesocket(client_sockets[socket_index]);
    deregister_client(socket_index);
    return (void*) 0;
}

int main() {
    clients_count = 0;
    clients_array_size = 256;
    client_sockets = malloc(sizeof(SOCKET) * clients_array_size);

    WSADATA wsa_data;
    int result_code;
    // Initialize Winsock
    result_code = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result_code != 0) {
        printf("WSAStartup failed: %d\n", result_code);
        return 1;
    }

    /* Initialize socket structure */
    struct addrinfo *result = NULL, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result_code = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (result_code != 0) {
        printf("getaddrinfo failed: %d\n", result_code);
        WSACleanup();
        return 1;
    }

    SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    /* Now bind the host address using bind() call.*/
    result_code = bind(listen_socket, result->ai_addr, (int) result->ai_addrlen);
    if (result_code == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    result_code = listen(listen_socket, SOMAXCONN);
    if (result_code == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    SOCKET client_socket;
    while (1) {
        /* Accept actual connection from the client */
        client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(listen_socket);
            WSACleanup();
            return 1;
        }
        else {
            int socket_index = register_client(client_socket);
            pthread_t client_thread;
            int client_thread_code = pthread_create(
                &client_thread, NULL, client_handling_routine, (void*) socket_index);
            if (client_thread_code < 0) {
                perror("ERROR on starting a pthread");
            }
        }
    }
}