#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <assert.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <string.h>

const uint16_t PORT = 5001;
#define CLIENTS_CAPACITY 5
#define LOGIN_CAPACITY 8
#define BUFFER_CAPACITY 4096
#define TIME_OFFSET 19
int server_socket;
int clients[CLIENTS_CAPACITY];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char buffer[BUFFER_CAPACITY];
char login[CLIENTS_CAPACITY][LOGIN_CAPACITY];
struct sockaddr_in server_address, client_address;

void initialize_server_socket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset((char*)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    listen(server_socket, CLIENTS_CAPACITY);
}

int find_slot_for_client(int client_socket) {
    int index = -1;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        if (!clients[i]) {
            clients[i] = client_socket;
            index = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return index;
}

void clean_without_lock(int index) {
    memset(login[index], 0, sizeof(login[index]));
    close(clients[index]);
    clients[index] = 0;
}

void clean(int index) {
    pthread_mutex_lock(&mutex);
    clean_without_lock(index);
    pthread_mutex_unlock(&mutex);
}

void broadcast(int index, char* buffer, int bytes) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        if (!clients[i]) {
            continue;
        }
        write(clients[i], buffer, bytes);
    }
    pthread_mutex_unlock(&mutex);
}

void* client_handling_routine(void* arg) {
    int client_socket = (int)arg;
    int index = find_slot_for_client(client_socket);
    if (index == -1) {
        write(client_socket, "-", 1);
        close(client_socket);
        return NULL;
    } else {
        write(client_socket, "+", 1);
    }
    ssize_t bytes_read = read(client_socket, login[index], LOGIN_CAPACITY - 1);
    if (bytes_read <= 0) {
        perror("ERROR on login reading");
        clean(index);
        return NULL;
    }
    char buffer[BUFFER_CAPACITY];
    int offset = strlen(login[index]) + 4 + TIME_OFFSET;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = read(client_socket, buffer + offset, BUFFER_CAPACITY - 1);
        if (bytes_read <= 0 || buffer[offset] == 1) {
            break;
        }
        time_t orig_format;
        time(&orig_format);
        memcpy(buffer, asctime(gmtime(&orig_format)), TIME_OFFSET);
        buffer[TIME_OFFSET] = '[';
        memcpy(buffer + TIME_OFFSET + 1, login[index], strlen(login[index]));
        buffer[TIME_OFFSET + 1 + strlen(login[index])] = ']';
        buffer[TIME_OFFSET + 2 + strlen(login[index])] = ':';
        buffer[TIME_OFFSET + 3 + strlen(login[index])] = ' ';
        broadcast(index, buffer, bytes_read + offset);
    }
    clean(index);
}

int main(int argc, char *argv[]) {
    initialize_server_socket();
    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        memset(login[i], 0, sizeof(login[i]));
    }
    unsigned int size_of_client_address = sizeof(client_address);
    while (1) {
        int client_socket = accept(
                server_socket,
                (struct sockaddr *) &client_address,
                &size_of_client_address
        );
        pthread_t client_thread;
        pthread_create(
                &client_thread,
                NULL,
                client_handling_routine,
                client_socket
        );
    }
}