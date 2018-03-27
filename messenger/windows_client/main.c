#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>

void* server_updates(void* arg) {
    SOCKET* sock_p = (SOCKET *) arg;

    uint8_t nickname_len = 0, message_len = 0;
    char nickname[300];
    char message[300];
    char time[300];
    memset(message, 0, 300);
    memset(nickname, 0, 300);
    memset(time, 0, 300);

    while (1) {
        pthread_testcancel();

        if (recv(*sock_p, nickname, 1, 0) != 1) {
            perror("ERROR nickname len reading from socket");
            return (void*) 1;
        }
        nickname_len = (uint8_t) nickname[0];
        nickname[0] = 0;

        if (recv(*sock_p, nickname, nickname_len, 0) != nickname_len) {
            perror("ERROR nickname reading from socket");
            return (void*) 1;
        }

        if (recv(*sock_p, message, 1, 0) != 1) {
            perror("ERROR message len reading from socket");
            return (void*) 1;
        }
        message_len = (uint8_t) message[0];
        message[0] = 0;

        if (recv(*sock_p, message, message_len, 0) != message_len) {
            perror("ERROR message reading from socket");
            return (void*) 1;
        }

        if (recv(*sock_p, time, 9, 0) != 9) {
            perror("ERROR time reading from socket");
            return (void*) 1;
        }

        printf("[%s] %s says: %s\n", nickname, time, message);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage: hostname port nickname\n");
        return 0;
    }

    WSADATA wsa_data;
    int code = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (code != 0) {
        printf("WSAStartup failed: %d\n", code);
        return 1;
    }

    struct addrinfo* result = NULL, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    code = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (code != 0) {
        printf("getaddrinfo failed: %d\n", code);
        WSACleanup();
        return 1;
    }

    SOCKET connect_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (connect_socket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    code = connect(connect_socket, result->ai_addr, (int) result->ai_addrlen);
    if (code == SOCKET_ERROR) {
        closesocket(connect_socket);
        connect_socket = INVALID_SOCKET;
    }
    freeaddrinfo(result);
    if (connect_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    char buffer[129];
    memset(buffer, 0, 129);
    uint8_t nickname_len = (uint8_t) strlen(argv[3]);
    buffer[0] = nickname_len;
    printf("nickname len %d\n", nickname_len);
    strcpy(buffer + 1, argv[3]);
    if (send(connect_socket, buffer, strlen(buffer), 0) != 1 + nickname_len) {
        printf("Error writing nickname\n");
        return 1;
    }

    printf("sent nickname: %s\n", buffer);
    printf("Entered to chat, press :q to exit:\n");

    pthread_t updates_thread;
    int p_code = pthread_create(&updates_thread, NULL, server_updates, (void*) &connect_socket);
    if (p_code < 0) {
        perror("ERROR on creating a pthread");
        exit(1);
    }

    while(1) {
        printf("Enter your message(and press enter in the end of it):\n");

        char message[128];
        memset(message, 0, 128);
        fgets(message, 128, stdin);

        if (strcmp(message, ":q\n") == 0) {
            break;
        }

        uint8_t message_len = (uint8_t) strlen(message);
        buffer[0] = message_len;
        strcpy(buffer + 1, message);
        if (send(connect_socket, buffer, strlen(buffer), 0) != 1 + message_len) {
            perror("ERROR writing to socket");
            exit(1);
        }

        printf("sent message\n");
    }

    pthread_cancel(updates_thread);
    pthread_join(updates_thread, NULL);

    int shutdown_code = shutdown(connect_socket, SD_BOTH);
    if (shutdown_code == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }
    closesocket(connect_socket);
    WSACleanup();

    return 0;
}