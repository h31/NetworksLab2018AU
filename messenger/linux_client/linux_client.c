#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>
#include <time.h>

uint16_t port;
#define LOGIN_CAPACITY 7
#define BUFFER_CAPACITY 4095
int client_socket;
volatile char typing = 0;
struct sockaddr_in server_address;
struct hostent *server;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
const char* CLIENT_GREETING =
        "Welcome\nm + Enter - don't receive other clients' messages while typing yours\nexit - leave chat\n";
const char* TOO_MANY_CLIENTS = "Too many clients. You will be disconnected\n";
char buffer[BUFFER_CAPACITY];
char *login;
const char* EXIT = "exit\n";
const char* YES = "yes\n";
const char* YES_LEAVE_CHAT = "yes - leave chat\n";

void read_arguments(int argc, char *argv[]) {
    if (argc < 3 || strlen(argv[3]) > LOGIN_CAPACITY) {
        printf("usage %s hostname port login\nport=5001\n|login| <= %d\n", argv[0], LOGIN_CAPACITY);
        exit(0);
    }
    server = gethostbyname(argv[1]);
    port = (uint16_t)atoi(argv[2]);
    login = argv[3];
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
    char buffer[BUFFER_CAPACITY];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, BUFFER_CAPACITY - 1);
        pthread_mutex_lock(&mutex);
        while (typing) {
            pthread_cond_wait(&cond, &mutex);
        }
        printf(buffer);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    read_arguments(argc, argv);
    setup_connection();

    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, BUFFER_CAPACITY - 1);
    if (buffer[0] != '+') {
        printf(TOO_MANY_CLIENTS);
        close(client_socket);
        return 0;
    } else {
        printf(CLIENT_GREETING);
    }
    write(client_socket, login, strlen(login));
    pthread_t reading_thread;
    pthread_create(&reading_thread, NULL, reading_routine, client_socket);
    while (1) {
        char buffer[BUFFER_CAPACITY];
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, BUFFER_CAPACITY - 1, stdin);
        if (strcmp(buffer, EXIT) == 0) {
            printf(YES_LEAVE_CHAT);
            fgets(buffer, BUFFER_CAPACITY - 1, stdin);
            if (strcmp(buffer, YES) == 0) {
                memset(buffer, 0, sizeof(buffer));
                buffer[0] = 1;
                write(client_socket, buffer, 1);
                break;
            } else {
                memcpy(buffer, EXIT, strlen(EXIT));
            }
        }
        if (!typing && strcmp(buffer, "m\n") == 0) {
            typing = 1;
            pthread_cond_broadcast(&cond);
            continue;
        }
        typing = 0;
        pthread_cond_broadcast(&cond);
        write(client_socket, buffer, strlen(buffer));
    }
    close(client_socket);
    return 0;
}