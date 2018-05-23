#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>

const uint16_t PORT = 5002;
#define CLIENTS_CAPACITY 5
#define LOGIN_CAPACITY 16
#define BUFFER_CAPACITY 256
#define TIME_OFFSET 19
int server_socket;
int clients[CLIENTS_CAPACITY];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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

void broadcast(char* buffer, size_t bytes) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        if (!clients[i]) {
            continue;
        }
        ssize_t total_write_bytes = 0;
        while (total_write_bytes < bytes) {
            ssize_t write_bytes = write(clients[i], buffer, bytes);
            //printf("%zi %zi %zi\n", write_bytes, total_write_bytes, bytes);
            if (write_bytes <= 0) {
                //perror("WRITE ERROR");
                clients[i] = 0;
                break;
            }
            total_write_bytes += write_bytes;
        }
        //printf("BROADCASTED %s\n", buffer);
    }
    pthread_mutex_unlock(&mutex);
}

void* client_handling_routine(void* arg) {
    int client_socket = (int) arg;
    int index = find_slot_for_client(client_socket);
    if (index == -1) {
        write(client_socket, "-", 1);
        close(client_socket);
        return NULL;
    } else {
        write(client_socket, "+", 1);
    }

    char expected_bytes_tmp[1];
    ssize_t tmp = read(client_socket, expected_bytes_tmp, 1);
    if (tmp <= 0) {
        printf("READING ERROR");
        return NULL;
    }
    ssize_t expected_bytes = expected_bytes_tmp[0];
    ssize_t total_read_bytes = 0;
    while (total_read_bytes < expected_bytes) {
        ssize_t read_bytes = read(client_socket, login[index] + total_read_bytes, expected_bytes - total_read_bytes);
        if (read_bytes <= 0) {
            //perror("READING ERROR");
            clean(index);
            return NULL;
        }
        total_read_bytes += read_bytes;
    }

    char buffer[BUFFER_CAPACITY];
    size_t offset = strlen(login[index]) + 5 + TIME_OFFSET;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        expected_bytes_tmp[0] = 0;
        tmp = read(client_socket, expected_bytes_tmp, 1);
        if (tmp <= 0) {
            //perror("READING ERROR");
            return NULL;
        }
        expected_bytes = expected_bytes_tmp[0];
        total_read_bytes = 0;
        while (total_read_bytes < expected_bytes) {
            ssize_t read_bytes = read(
                    client_socket,
                    buffer + total_read_bytes + offset,
                    (size_t) (expected_bytes - total_read_bytes)
            );
            //printf("%zi %zi %zi\n", read_bytes, total_read_bytes, expected_bytes);
            if (read_bytes <= 0) {
                //perror("READING ERROR");
                clean(index);
                return NULL;
            }
            total_read_bytes += read_bytes;
        }

        time_t orig_format;
        time(&orig_format);
        buffer[0] = (char) (offset + total_read_bytes - 1);
        memcpy(buffer + 1, asctime(gmtime(&orig_format)), TIME_OFFSET);
        buffer[TIME_OFFSET + 1] = '[';
        memcpy(buffer + TIME_OFFSET + 2, login[index], strlen(login[index]));
        buffer[TIME_OFFSET + 2 + strlen(login[index])] = ']';
        buffer[TIME_OFFSET + 3 + strlen(login[index])] = ':';
        buffer[TIME_OFFSET + 4 + strlen(login[index])] = ' ';
        //puts(buffer);
        broadcast(buffer, offset + total_read_bytes);
    }
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