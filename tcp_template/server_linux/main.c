#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <netdb.h>
#include <unistd.h>

#include <string.h>

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int* client_socket_ids;
int clients_array_size, clients_count;

void send_message_to_everyone(char* login, char* message, int sender_socket_id) {
    uint8_t login_length = (uint8_t) strlen(login);
    uint8_t message_length = (uint8_t) strlen(message);
    char buffer[512];
    buffer[0] = login_length;
    buffer[1] = message_length;
    strcpy(buffer + 2, login);
    strcpy(buffer + 2 + login_length, message);
    pthread_mutex_lock(&clients_mutex);
    printf("Sending message from %d\n", sender_socket_id);
    for (int i = 0; i < clients_count; i++) {
        int socket_id = client_socket_ids[i];
        if (socket_id == sender_socket_id) {
            continue;
        }
        ssize_t n = write(socket_id, buffer, strlen(buffer));
        if (n < 0) {
            printf("Could not write message to %d\n", socket_id);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void register_client(int socket_id) {
    pthread_mutex_lock(&clients_mutex);
    if (clients_count == clients_array_size) {
        clients_array_size *= 2;
        client_socket_ids = realloc(client_socket_ids, sizeof(int) * clients_array_size);
    }
    client_socket_ids[clients_count] = socket_id;
    clients_count += 1;
    printf("Client with socket %d was registered\n", socket_id);
    pthread_mutex_unlock(&clients_mutex);
}

void deregister_client(int socket_id) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < clients_count; i++) {
        if (client_socket_ids[i] == socket_id) {
            client_socket_ids[i] = client_socket_ids[clients_count - 1];
            clients_count -= 1;
            break;
        }
    }
    printf("Client with socket %d was deregistered\n", socket_id);
    pthread_mutex_unlock(&clients_mutex);
}

void inner_client_handling(int socket_id) {
    /* Reading login */
    char login[256];
    bzero(login, 256);
    if (read(socket_id, login, 1) != 1) {
        printf("Connection with %d will be closed due to login failure\n", socket_id);
        return;
    }
    uint8_t login_length = (uint8_t) login[0];
    login[0] = 0;
    if (read(socket_id, login, login_length) != login_length) {
        printf("Connection with %d will be closed due to login failure\n", socket_id);
        return;
    }
    /* If login is read then start communicating */
    printf("Socket id %d is %s\n", socket_id, login);
    char buffer[256];
    while (1) {
        bzero(buffer, 256);
        if (read(socket_id, buffer, 1) != 1) {
            printf("Connection with %d will be closed due to read() error\n", socket_id);
            return;
        }
        uint8_t buffer_length = (uint8_t) buffer[0];
        buffer[0] = 0;
        if (read(socket_id, buffer, buffer_length) != buffer_length) {
            printf("Connection with %d will be closed due to read() error\n", socket_id);
            return;
        }
        printf("Received the message from %d: %s\n", socket_id, buffer);
        send_message_to_everyone(login, buffer, socket_id);
    }
}

void* client_handling_routine(void* arg) {
    int socket_id = (int) arg;
    register_client(socket_id);
    inner_client_handling(socket_id);
    deregister_client(socket_id);
    if (close(socket_id) < 0) {
        printf("ERROR closing the socket %d\n", socket_id);
        return (void*) 1;
    }
    return (void*) 0;
}

int main(int argc, char *argv[]) {
    clients_count = 0;
    clients_array_size = 256;
    client_socket_ids = malloc(sizeof(int) * clients_array_size);

    /* First call to socket() function */
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_socket < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    uint16_t port_number = 5001;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    /* Now bind the host address using bind() call.*/
    if (bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    listen(listen_socket, 5);
    struct sockaddr_in cli_addr;
    unsigned int clilen = sizeof(cli_addr);
    while (1) {
        /* Accept actual connection from the client */
        int client_socket = accept(listen_socket, (struct sockaddr *) &cli_addr, &clilen);
        if (client_socket < 0) {
            perror("ERROR on accept");
        }
        else {
            pthread_t client_thread;
            int client_thread_code = pthread_create(
                &client_thread, NULL, client_handling_routine, (void*) client_socket);
            if (client_thread_code < 0) {
                perror("ERROR on starting a pthread");
            }
        }
    }
}