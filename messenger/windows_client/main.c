#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

void* server_updates(void* arg) {
    int server_socket = (int) arg;

    uint8_t nickname_len = 0, message_len = 0;
    char nickname[300];
    char message[300];
    char time[300];
    bzero(message, 300);
    bzero(nickname, 300);
    bzero(time, 300);

    while (1) {
        pthread_testcancel();

        if (read(server_socket, nickname, 1) != 1) {
            perror("ERROR nickname len reading from socket");
            return (void*) 1;
        }
        nickname_len = nickname[0];
        nickname[0] = 0;

        if (read(server_socket, nickname, nickname_len) != nickname_len) {
            perror("ERROR nickname reading from socket");
            return (void*) 1;
        }

        if (read(server_socket, message, 1) != 1) {
            perror("ERROR message len reading from socket");
            return (void*) 1;
        }
        message_len = message[0];
        message[0] = 0;

        if (read(server_socket, message, message_len) != message_len) {
            perror("ERROR message reading from socket");
            return (void*) 1;
        }

        if (read(server_socket, time, 9) != 9) {
            perror("ERROR time reading from socket");
            return (void*) 1;
        }

        printf("[%s] %s says: %s\n", nickname, time, message);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    uint16_t port_number;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4) {
        printf("usage: hostname, port, nickname\n");
        exit(0);
    }
    port_number = (uint16_t) atoi(argv[2]);

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr, (size_t)server->h_length);
    serv_addr.sin_port = htons(port_number);

    if (connect(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    printf("connected to server\n");

    char buffer[128];
    bzero(buffer, 128);
    uint8_t nickname_len = (uint8_t) strlen(argv[3]);
    buffer[0] = nickname_len;
    strcpy(buffer + 1, argv[3]);
    if (write(socketfd, buffer, strlen(buffer)) != 1 + nickname_len) {
        perror("ERROR writing to socket");
        exit(1);
    }

    pthread_t updates_thread;
    int code = pthread_create(&updates_thread, NULL, server_updates, (void*) socketfd);
    if (code < 0) {
        perror("ERROR on creating a pthread");
        exit(1);
    }

    printf("sent nickname: %s\n", buffer);
    printf("Entered to chat, press :q to exit:\n");

    while(1) {
        printf("Enter your message(and press enter in the end of it):\n");

        char message[128];
        bzero(message, 128);
        fgets(message, 128, stdin);

        if (strcmp(message, ":q\n") == 0) {
            break;
        }

        uint8_t message_len = (uint8_t) strlen(message);
        buffer[0] = message_len;
        strcpy(buffer + 1, message);
        if (write(socketfd, buffer, strlen(buffer)) != 1 + message_len) {
            perror("ERROR writing to socket");
            exit(1);
        }

        printf("sent message\n", message);
    }

    pthread_cancel(updates_thread);
    pthread_join(updates_thread, NULL);

    if (close(socketfd) < 0) {
        perror("ERROR on closing the socket");
        exit(1);
    }

    return 0;
}