#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>
#include <time.h>

SOCKET* client_ids;
size_t clients_count = 0, max_count;
pthread_mutex_t mutex;

void resize_client_ids() {
    SOCKET* new_ids = malloc((max_count * 2));

    for (size_t i = 0; i < clients_count; i++) {
        new_ids[i] = client_ids[i];
    }

    free(client_ids);
    max_count *= 2;

    client_ids = new_ids;
}


int add_client(SOCKET socket) {
    if (clients_count == max_count) {
        resize_client_ids();
    }

    printf("clients count %d\n", clients_count);
    int index = clients_count;
    client_ids[index] = socket;
    clients_count++;

    pthread_mutex_unlock(&mutex);
    printf("Client with socket index %d was registered\n", index);
    return index;
}

void send_message(char* nickname, char* message, char* time) {
    printf("sending message to everyone from %s\n", nickname);
    uint8_t nickname_len = (uint8_t) strlen(nickname);
    uint8_t message_len = (uint8_t) strlen(message);

    // 1 + 128 + 1 + 128 + 10 = 268
    char buffer[269];
    memset(buffer, 0, 269);
    buffer[0] = nickname_len;
    strcpy(buffer + 1, nickname);

    buffer[1 + nickname_len] = message_len;
    strcpy(buffer + 1 + nickname_len + 1, message);
    strcpy(buffer + 1 + nickname_len + 1 + message_len, time);

    for (size_t i = 0; i < clients_count; i++) {
        printf("sending to client %zu with socket %d\n", i, client_ids[i]);
        if (client_ids[i] == INVALID_SOCKET) {
            continue;
        }

        ssize_t n = send(client_ids[i], buffer, strlen(buffer), 0);
        if (n < 0) {
            perror("ERROR writing to socket");
        }
    }

    printf("message sent\n");
}

void handle_client_input(int socket) {
    char nickname[128];
    memset(nickname, 0, 128);
    SOCKET sock = client_ids[socket];

    if (recv(sock, nickname, 1, 0) != 1) {
        printf("ERROR reading from %d socket length of nickname\n", socket);
        return;
    }

    uint8_t nickname_len = (uint8_t) nickname[0];
    nickname[0] = 0;

    printf("nickname len %d\n", nickname_len);
    if (recv(sock, nickname, nickname_len, 0) != nickname_len) {
        printf("ERROR reading client nickname\n");
        return;
    }

    printf("received client's nickname: %s\n", nickname);

    char buffer[128];
    while (1) {
        memset(buffer, 0, 128);
        if (recv(sock, buffer, 1, 0) != 1) {
            printf("ERROR reading from %d socket message length\n", socket);
            return;
        }

        uint8_t buffer_length = (uint8_t) buffer[0];
        buffer[0] = 0;

        if (recv(sock, buffer, buffer_length, 0) != buffer_length) {
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

    pthread_mutex_lock(&mutex);
    handle_client_input(client_socket);
    client_ids[client_socket] = INVALID_SOCKET;
    pthread_mutex_unlock(&mutex);


    return (void*) 0;
}


int main(int argc, char *argv[]) {
    printf("creating a server\n");

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

    result_code = getaddrinfo(NULL, "9000", &hints, &result);
    if (result_code != 0) {
        printf("getaddrinfo error: %d\n", result_code);
        WSACleanup();
        return 1;
    }

    SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        printf("Error socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    /* Now bind the host address using bind() call.*/
    result_code = bind(listen_socket, result->ai_addr, (int) result->ai_addrlen);
    if (result_code == SOCKET_ERROR) {
        printf("bind error: %d\n", WSAGetLastError());
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
        printf("Listen error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    SOCKET client_socket;

    pthread_mutex_init(&mutex, NULL);
    max_count = 10;
    client_ids = malloc(sizeof(SOCKET) * max_count);
    printf("listening on port %d\n", 9000);
    while (1) {
        client_socket = accept(listen_socket, NULL, NULL);

        if (client_socket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(listen_socket);
            WSACleanup();
            return 1;
        } else {
            int socket_index = add_client(client_socket);
            pthread_t client_thread;
            int code = pthread_create(&client_thread, NULL, handle_client, (void*) socket_index);

            if (code < 0) {
                perror("ERROR on starting a pthread");
            }
        }
    }
}