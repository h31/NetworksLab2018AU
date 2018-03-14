#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

volatile int message_entering_mode = 0;
volatile int should_finish = 0;

void *server_updates_routine(void *arg) {
    SOCKET *socket_ptr = (SOCKET *) arg;
    char message[256];
    char login[256];
    char time[9];
    time[2] = ':';
    time[5] = ':';
    time[8] = 0;
    while (!should_finish) {
        memset(message, 0, 256);
        memset(login, 0, 256);
        if (recv(*socket_ptr, message, 8, 0) != 8) {
            if (should_finish) {
                break;
            }
            printf("ERROR reading from socket\n");
            return (void*) 1;
        }
        uint8_t login_length = (uint8_t) message[0];
        uint8_t message_length = (uint8_t) message[1];
        time[0] = message[2]; // H
        time[1] = message[3]; // H
        time[3] = message[4]; // M
        time[4] = message[5]; // M
        time[6] = message[6]; // S
        time[7] = message[7]; // S
        memset(message, 0, 8);
        if (recv(*socket_ptr, login, login_length, 0) != login_length) {
            if (should_finish) {
                break;
            }
            printf("ERROR reading from socket\n");
            return (void*) 1;
        }
        if (recv(*socket_ptr, message, message_length, 0) != message_length) {
            if (should_finish) {
                break;
            }
            printf("ERROR reading from socket\n");
            return (void*) 1;
        }
        /* Waiting for the user to stop entering */
        while (message_entering_mode && !should_finish);
        printf("<%s> [%s] %s\n", time, login, message);
    }
    return (void*) 0;
}

void communicating_routine(SOCKET connect_socket, char *login) {
    /* Sending login to the server */
    char buffer[256];
    memset(buffer, 0, 256);
    uint8_t login_length = (uint8_t) strlen(login);
    buffer[0] = login_length;
    strcpy(buffer + 1, login);
    if (send(connect_socket, buffer, strlen(buffer), 0) != 1 + login_length) {
        printf("Error on writing the login\n");
        return;
    }

    pthread_t updates_thread;
    int code = pthread_create(&updates_thread, NULL, server_updates_routine, (void*) &connect_socket);
    if (code < 0) {
        printf("ERROR on creating a pthread: %d\n", code);
        return;
    }
    printf("Nice chatting! Enter:\n");
    printf("\t:m to not be interrupted while you write\n");
    printf("\t:q to quit\n");

    char input[257];
    /* Now ask for a message from the user, this message will be read by server */
    while (!should_finish) {
        memset(input, 0, 257);
        fgets(input, 256, stdin);
        if (strcmp(input, ":q\n") == 0) {
            should_finish = 1;
            continue;
        }
        if (strcmp(input, ":m\n") == 0) {
            message_entering_mode = 1;
            continue;
        }
        /* Remove end of line character */
        input[strlen(input) - 1] = 0;

        /* Send message to the server */
        uint8_t message_length = (uint8_t) strlen(input);
        buffer[0] = message_length;
        strcpy(buffer + 1, input);
        if (send(connect_socket, buffer, strlen(buffer), 0) != 1 + message_length) {
            printf("Error on writing a message\n");
            return;
        }
        message_entering_mode = 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port nickname\n", argv[0]);
        return 0;
    }
    
    // Initialize Winsock
    WSADATA wsa_data;
    int result_code = WSAStartup(MAKEWORD(2,2), &wsa_data);
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

    result_code = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (result_code != 0) {
        printf("getaddrinfo failed: %d\n", result_code);
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

    result_code = connect(connect_socket, result->ai_addr, (int) result->ai_addrlen);
    if (result_code == SOCKET_ERROR) {
        closesocket(connect_socket);
        connect_socket = INVALID_SOCKET;
    }
    freeaddrinfo(result);
    if (connect_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    communicating_routine(connect_socket, argv[3]);

    result_code = shutdown(connect_socket, SD_BOTH);
    if (result_code == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }
    closesocket(connect_socket);
    WSACleanup();
    return 0;
}