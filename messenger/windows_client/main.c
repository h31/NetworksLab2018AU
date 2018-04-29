#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>

#include <pthread.h>

#include <string.h>

#include <winsock2.h>
#include <Ws2tcpip.h>


#define MAX_LOGIN_LEN 256
#define MAX_MSG_LEN 256

volatile int print_pause = 0;

int read_from_socket(SOCKET sockfd, char *buffer, int len, int sz) {
    memset(buffer, 0, sz);
    int result = recv(sockfd, buffer, len, 0); 
    if (result == SOCKET_ERROR) {
        printf("Error during read: %d\n", WSAGetLastError());
    }
    return result;
}

int send_via_socket(SOCKET sockfd, char *buffer) {
    int result = send(sockfd, buffer, strlen(buffer), 0);
    if (result == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
    }
    return result;
} 

void *receiving_messages(void *arg) {
    SOCKET sockfd = *((int *)arg);

    char header[4], login[MAX_LOGIN_LEN], msg[MAX_MSG_LEN];
    while (read_from_socket(sockfd, header, 4, 4) != SOCKET_ERROR) {
        uint8_t hours, mins, login_len, msg_len;
        memcpy(&hours, header, 1);
        memcpy(&mins, header + 1, 1);
        memcpy(&login_len, header + 2, 1);
        memcpy(&msg_len, header + 3, 1);
        if (read_from_socket(sockfd, login, login_len, MAX_LOGIN_LEN) == SOCKET_ERROR) return NULL;
        if (read_from_socket(sockfd, msg, msg_len, MAX_MSG_LEN) == SOCKET_ERROR) return NULL;
        while (print_pause) {
            Sleep(1);
        }
        printf("<%02d:%02d> [%s] %s", hours, mins, login, msg);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port login\n", argv[0]);
        exit(0);
    }
    uint16_t portno = (uint16_t) atoi(argv[2]);

    char login[MAX_LOGIN_LEN];
    strcpy(login, argv[3]);

    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != NO_ERROR) {
        printf("WSAStartup failed: %d\n", result);
        exit(0);
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    result = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); 
    if (result == SOCKET_ERROR) {
        closesocket(sockfd);
        printf("Unable to connect to server: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }


    if (send_via_socket(sockfd, login) == SOCKET_ERROR) {
        exit(1);
    }
    pthread_t client;
    pthread_create(&client, NULL, receiving_messages, (void *) &sockfd);

    char buffer[MAX_MSG_LEN];
    while (1) {
        memset(buffer, 0, MAX_MSG_LEN);
        fgets(buffer, MAX_MSG_LEN - 1, stdin);  
        if (strcmp(buffer, "m\n") == 0) {
            memset(buffer, 0, MAX_MSG_LEN);
            print_pause = 1;
            
            time_t now = time(NULL);
            struct tm *now_tm = localtime(&now);
            int hours = now_tm->tm_hour;
            int mins = now_tm->tm_min;
            printf("<%02d:%02d> [%s] ", hours, mins, login);
            fgets(buffer, MAX_MSG_LEN - 1, stdin);    
            print_pause = 0;
            if (send_via_socket(sockfd, buffer) == SOCKET_ERROR) {
                exit(1);
            }
        } 
    }

    pthread_join(client, NULL);
    closesocket(sockfd);
    WSACleanup();    
    return 0;
}