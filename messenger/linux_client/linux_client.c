#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>

uint16_t port;
#define LOGIN_CAPACITY 16
#define BUFFER_CAPACITY 256
#define MESSAGE_MAX_LENGTH 100
int client_socket;
volatile char typing = 0;
struct sockaddr_in server_address;
struct hostent *server;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
const char* CLIENT_GREETING =
        "Welcome\nm + Enter - don't receive other clients' messages while typing yours\nexit - leave chat\n";
const char* TOO_MANY_CLIENTS = "Too many clients. You will be disconnected\n";
char login[LOGIN_CAPACITY];
const char* EXIT = "exit\n";
const char* YES = "yes\n";
const char* YES_LEAVE_CHAT = "yes - leave chat\n";

void read_arguments(int argc, char *argv[]) {
    if (argc < 3 || strlen(argv[3]) > LOGIN_CAPACITY - 2) {
        printf("usage %s hostname port login\nport=5001\n|login| <= %d\n", argv[0], LOGIN_CAPACITY - 2);
        exit(0);
    }
    server = gethostbyname(argv[1]);
    port = (uint16_t) atoi(argv[2]);
    login[0] = (char) strlen(argv[3]);
    memcpy(login + 1, argv[3], strlen(argv[3]));
}

void setup_connection() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset((char *)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy((char *)&server_address.sin_addr.s_addr,
           server->h_addr,
           (size_t)server->h_length
    );
    server_address.sin_port = htons(port);
    connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

void* reading_routine(void* arg) {
    int client_socket = (int) arg;
    char buffer[BUFFER_CAPACITY];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        char expected_bytes_tmp[1];
        expected_bytes_tmp[0] = 0;
        //printf("READING EXP\n");
        if (read(client_socket, expected_bytes_tmp, 1) <= 0) {
            perror("READING ERROR");
            exit(1);
        }
        ssize_t expected_bytes = expected_bytes_tmp[0];
        ssize_t total_read_bytes = 0;
        while (total_read_bytes < expected_bytes) {
            //printf("ITER\n");
            ssize_t read_bytes = read(
                    client_socket,
                    buffer + total_read_bytes,
                    (size_t) (expected_bytes - total_read_bytes)
            );
            //printf("%zi %zi %zi\n", read_bytes, total_read_bytes, expected_bytes);
            if (read_bytes <= 0) {
                perror("READING ERROR");
                exit(1);
            }
            total_read_bytes += read_bytes;
            //printf("%zi %zi %zi\n", read_bytes, total_read_bytes, expected_bytes);
        }
        //printf("PRETYPING\n");
        pthread_mutex_lock(&mutex);
        while (typing) {
            //printf("TYPING\n");
            pthread_cond_wait(&cond, &mutex);
        }
        //printf("AFTERTYPING\n");
        printf("%s\n", buffer);
        //printf("AFTERPRINT");
        pthread_mutex_unlock(&mutex);

    }
}

int main(int argc, char *argv[]) {
    read_arguments(argc, argv);
    setup_connection();
    char buffer[BUFFER_CAPACITY];
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, 1);
    if (buffer[0] != '+') {
        printf(TOO_MANY_CLIENTS);
        close(client_socket);
        return 0;
    } else {
        printf(CLIENT_GREETING);
    }
    write(client_socket, login, strlen(login));
    pthread_t reading_thread;
    pthread_create(&reading_thread, NULL, reading_routine, (void *) client_socket);
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer + 1, MESSAGE_MAX_LENGTH, stdin);
        if (strcmp(buffer, EXIT) == 0) {
            printf(YES_LEAVE_CHAT);
            fgets(buffer, MESSAGE_MAX_LENGTH, stdin);
            if (strcmp(buffer, YES) == 0) {
                break;
            } else {
                memcpy(buffer + 1, EXIT, strlen(EXIT));
            }
        }
        if (!typing && strcmp(buffer + 1, "m\n") == 0) {
            typing = 1;
            //printf("IF BROAD");
            pthread_cond_broadcast(&cond);
            continue;
        }
        typing = 0;
        //printf("BROAD");
        pthread_cond_broadcast(&cond);
        buffer[0] = (char) (strlen(buffer + 1) - 1);
        write(client_socket, buffer, (size_t) (buffer[0] + 1));
    }
    close(client_socket);
    return 0;
}