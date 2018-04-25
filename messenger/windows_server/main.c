#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>

#include <pthread.h>

#include <string.h>

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <time.h>

#define MAX_CLIENTS 10
#define MAX_LOGIN_LEN 256
#define MAX_MSG_LEN 256


pthread_mutex_t mmutex = PTHREAD_MUTEX_INITIALIZER;

SOCKET socket_fds[MAX_CLIENTS];

void init_socket_fds() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        socket_fds[i] = INVALID_SOCKET;
    }
}

void add_socket_fd(SOCKET socket_fd) {
    pthread_mutex_lock(&mmutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_fds[i] == INVALID_SOCKET) {
            socket_fds[i] = socket_fd;
            break;
        }
    }
    pthread_mutex_unlock(&mmutex);
}

void close_socket(SOCKET socket_fd) {
    pthread_mutex_lock(&mmutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_fds[i] == socket_fd) {
            socket_fds[i] = INVALID_SOCKET;
            break;
        }
    }
    closesocket(socket_fd);
    pthread_mutex_unlock(&mmutex);
}


int read_from_socket(SOCKET sockfd, char *buffer, int len, int sz) {
    memset(buffer, 0, sz);
    int result = recv(sockfd, buffer, len, 0); 
    if (result == SOCKET_ERROR) {
        printf("Error during read: %d\n", WSAGetLastError());
    }
    return result;
}

void send_all_clients(SOCKET socketfd, uint8_t hours, uint8_t mins, char *login, char *msg) {
    uint8_t login_len = strlen(login);
    uint8_t msg_len = strlen(msg);
    
    char header[4];
    memcpy(header, &hours, 1);
    memcpy(header + 1, &mins, 1);
    memcpy(header + 2, &login_len, 1);
    memcpy(header + 3, &msg_len, 1);
    
    pthread_mutex_lock(&mmutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_fds[i] != INVALID_SOCKET && socket_fds[i] != socketfd) {
            send(socket_fds[i], header, 4, 0);
            send(socket_fds[i], login, login_len, 0);
            send(socket_fds[i], msg, msg_len, 0);
        }
    }
    pthread_mutex_unlock(&mmutex);
}

void *connection_processing(void *arg) {
    SOCKET sockfd = *((int *)arg);
    
    char login[MAX_LOGIN_LEN];
    if (read_from_socket(sockfd, login, MAX_LOGIN_LEN, MAX_LOGIN_LEN) == SOCKET_ERROR) {
        close_socket(sockfd);
        return NULL;
    }

    char receiving_buffer[MAX_MSG_LEN];
    while (read_from_socket(sockfd, receiving_buffer, MAX_MSG_LEN, MAX_MSG_LEN) != SOCKET_ERROR) {
        time_t now = time(NULL);
        struct tm *now_tm = localtime(&now);
        int hours = now_tm->tm_hour;
        int mins = now_tm->tm_min;
        
        send_all_clients(sockfd, hours, mins, login, receiving_buffer);
    }
    close_socket(sockfd);
    return NULL;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(0);
    }
    uint16_t portno = atoi(argv[1]);

    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != NO_ERROR) {
        printf("WSAStartup failed: %d\n", result);
        exit(1);
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    result = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) {
        printf("Error at bind(): %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    listen(sockfd, MAX_CLIENTS);
    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);

    init_socket_fds();

    while (1) {
        SOCKET newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd == INVALID_SOCKET) {
            printf("Error on acceot: %d\n", WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        add_socket_fd(newsockfd);

        pthread_t client;
        pthread_create(&client, NULL, connection_processing, (void *) &newsockfd);
    }

    closesocket(sockfd);
    return 0;
}
