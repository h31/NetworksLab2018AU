#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <netdb.h>
#include <unistd.h>

#include <string.h>

volatile int message_entering_mode = 0;
volatile int should_finish = 0;

void* server_updates_routine(void* arg) {
    int socket_id = (int) arg;
    char message[256];
    char login[256];
    char time[9];
    time[2] = ':';
    time[5] = ':';
    time[8] = 0;
    while (!should_finish) {
        bzero(message, 256);
        bzero(login, 256);
        if (read(socket_id, message, 8) != 8) {
            if (should_finish) {
                break;
            }
            perror("ERROR reading from socket");
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
        bzero(message, 8);
        if (read(socket_id, login, login_length) != login_length) {
            if (should_finish) {
                break;
            }
            perror("ERROR reading from socket");
            return (void*) 1;
        }
        if (read(socket_id, message, message_length) != message_length) {
            if (should_finish) {
                break;
            }
            perror("ERROR reading from socket");
            return (void*) 1;
        }
        /* Waiting for the user to stop entering */
        while (message_entering_mode && !should_finish);
        printf("<%s> [%s] %s\n", time, login, message);
    }
    return (void*) 0;
}

int main(int argc, char *argv[]) {
    uint16_t port_number;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port nickname\n", argv[0]);
        exit(0);
    }
    port_number = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port_number);

    /* Now connect to the server */
    if (connect(socket_id, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    /* Sending login to the server */
    char buffer[256];
    bzero(buffer, 256);
    uint8_t login_length = (uint8_t) strlen(argv[3]);
    buffer[0] = login_length;
    strcpy(buffer + 1, argv[3]);
    if (write(socket_id, buffer, strlen(buffer)) != 1 + login_length) {
        perror("ERROR writing to socket");
        exit(1);
    }

    pthread_t updates_thread;
    int code = pthread_create(&updates_thread, NULL, server_updates_routine, (void*) socket_id);
    if (code < 0) {
        perror("ERROR on creating a pthread");
        exit(1);
    }
    printf("Nice chatting! Enter:\n");
    printf("\t:m to not be interrupted while you write\n");
    printf("\t:q to quit\n");

    char input[257];
    /* Now ask for a message from the user, this message will be read by server */
    while (!should_finish) {
        bzero(input, 257);
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
        if (write(socket_id, buffer, strlen(buffer)) != 1 + message_length) {
            perror("ERROR writing to socket");
            exit(1);
        }
        message_entering_mode = 0;
    }
    if (close(socket_id) < 0) {
        perror("ERROR on closing the socket");
        exit(1);
    }
    return 0;
}