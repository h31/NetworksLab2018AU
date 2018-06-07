#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int* client_ids;
size_t clients_count = 0, max_count;
pthread_mutex_t mutex;

void resize_client_ids() {
    int* new_ids = malloc((max_count * 2));

    for (size_t i = 0; i < clients_count; i++) {
        new_ids[i] = client_ids[i];
    }

    free(client_ids);
    max_count *= 2;

    client_ids = new_ids;
}


void add_client(int socket) {
    pthread_mutex_lock(&mutex);
    if (clients_count == max_count) {
        resize_client_ids();
    }

    printf("clients count %d\n", clients_count);
    client_ids[clients_count] = socket;
    clients_count++;
    pthread_mutex_unlock(&mutex);
}

void send_message(char* nickname, char* message, char* time) {
    printf("sending message to everyone from %s\n", nickname);
    uint8_t nickname_len = (uint8_t) strlen(nickname);
    uint8_t message_len = (uint8_t) strlen(message);

    // 1 + 128 + 1 + 128 + 10 = 268
    char buffer[269];
    bzero(buffer, 269);
    buffer[0] = nickname_len;
    strcpy(buffer + 1, nickname);

    buffer[1 + nickname_len] = message_len;
    strcpy(buffer + 1 + nickname_len + 1, message);
    strcpy(buffer + 1 + nickname_len + 1 + message_len, time);

    for (size_t i = 0; i < clients_count; i++) {
        printf("sending to client %zu with socket %d\n", i, client_ids[i]);
        int socket_dest = client_ids[i];

        ssize_t n = write(socket_dest, buffer, strlen(buffer));
        if (n < 0) {
            perror("ERROR writing to socket");
        }
    }

    printf("message sent\n");
}

void handle_client_input(int socket) {
    char nickname[128];
    bzero(nickname, 128);

    if (read(socket, nickname, 1) != 1) {
        printf("ERROR reading from %d socket length of nickname\n", socket);
        return;
    }

    ssize_t nickname_len = (ssize_t) nickname[0];
    nickname[0] = 0;
    if (read(socket, nickname, nickname_len) != nickname_len) {
        printf("ERROR reading client nickname\n");
        return;
    }

    printf("received client's nickname: %s\n", nickname);

    char buffer[128];
    bzero(buffer, 128);
    while (1) {
        bzero(buffer, 128);
        if (read(socket, buffer, 1) != 1) {
            printf("ERROR reading from %d socket message length\n", socket);
            return;
        }

        uint8_t buffer_length = (uint8_t) buffer[0];
        buffer[0] = 0;

        if (read(socket, buffer, buffer_length) != buffer_length) {
            printf("ERROR reading from %d socket message \n", socket);
            return;
        }

        printf("received client's message: %s\n", buffer);

        time_t rawtime;
        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);

        int hours = timeinfo->tm_hour;
        int minutes = timeinfo->tm_min;

        char current_time[10];
        printf("%d hours, %d minutes\n", hours, minutes);
        strftime(current_time, 10, "< %H:%M >", timeinfo);

        printf("current time is %s\n", current_time);
        send_message(nickname, buffer, current_time);
    }
}

void* handle_client(void* arg) {
    int client_socket = (int) arg;

    printf("adding client to server\n");
    add_client(client_socket);
    printf("added client to server\n");

    handle_client_input(client_socket);

    if (close(client_socket) < 0) {
        printf("ERROR closing the socket %d\n", client_socket);
        return (void*) 1;
    }

    return (void*) 0;
}


int main(int argc, char *argv[]) {
    printf("creating a server\n");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    uint16_t port_number = 9000;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);
    max_count = 200;
    client_ids = malloc(sizeof(int) * max_count);

    listen(sockfd, 5);
    struct sockaddr_in cli_addr;
    unsigned int clilen = sizeof(cli_addr);

    printf("listening on port %d\n", port_number);
    while (1) {
        int client_socket = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        printf("accepted client socket %d\n", client_socket);

        if (client_socket < 0) {
            perror("ERROR on accept");
        } else {
            pthread_t client_thread;
            int code = pthread_create(&client_thread, NULL, handle_client, (void*) client_socket);

            if (code < 0) {
                perror("ERROR on creating a pthread");
                exit(1);
            }
        }
    }
}