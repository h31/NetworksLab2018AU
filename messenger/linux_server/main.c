#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>

#include <string.h>

#include <time.h>

#define MAX_CLIENTS 10
#define MAX_LOGIN_LEN 256
#define MAX_MSG_LEN 256


pthread_mutex_t mmutex = PTHREAD_MUTEX_INITIALIZER;

int socket_fds[MAX_CLIENTS];

void init_socket_fds() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        socket_fds[i] = -1;
    }
}

void add_socket_fd(int socket_fd) {
    pthread_mutex_lock(&mmutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_fds[i] == -1) {
            socket_fds[i] = socket_fd;
            break;
        }
    }
    pthread_mutex_unlock(&mmutex);
}

void close_socket(int socket_fd) {
    pthread_mutex_lock(&mmutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_fds[i] == socket_fd) {
            socket_fds[i] = -1;
            break;
        }
    }
    pthread_mutex_unlock(&mmutex);
}

void send_all_clients(int socketfd, uint8_t hours, uint8_t mins, char *login, char *msg) {
    uint8_t login_len = strlen(login);
    uint8_t msg_len = strlen(msg);
    
    char header[4];
    memcpy(header, &hours, 1);
    memcpy(header + 1, &mins, 1);
    memcpy(header + 2, &login_len, 1);
    memcpy(header + 3, &msg_len, 1);
    
    pthread_mutex_lock(&mmutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_fds[i] != -1 && socket_fds[i] != socketfd) {
            write(socket_fds[i], header, 4);
            write(socket_fds[i], login, login_len);
            write(socket_fds[i], msg, msg_len);
        }
    }
    pthread_mutex_unlock(&mmutex);
}

int read_from_socket(int sockfd, char *buffer, int len) {
    bzero(buffer, len);
    int n = read(sockfd, buffer, len - 1); 
    if (n < 0) perror("ERROR reading from socket");
    return n;
}

void *connection_processing(void *arg) {
    int sockfd = *((int *)arg);
    
    char login[MAX_LOGIN_LEN];
    if (read_from_socket(sockfd, login, MAX_LOGIN_LEN) <= 0) {
        close_socket(sockfd);
        return NULL;
    }

    char receiving_buffer[MAX_MSG_LEN];
    while (read_from_socket(sockfd, receiving_buffer, MAX_MSG_LEN) > 0) {
        time_t now = time(NULL);
        struct tm *now_tm = localtime(&now);
        int hours = now_tm->tm_hour;
        int mins = now_tm->tm_min;
        
        send_all_clients(sockfd, hours, mins, login, receiving_buffer);
    }
    close_socket(sockfd);
    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(0);
    }
    uint16_t portno = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, MAX_CLIENTS);
    struct sockaddr_in cli_addr;
    unsigned int clilen = sizeof(cli_addr);

    init_socket_fds();

    while (1) {
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            // break;
        }

        add_socket_fd(newsockfd);

        pthread_t client;
        pthread_create(&client, NULL, connection_processing, (void *) &newsockfd);

    }

    close(sockfd);



    return 0;
}
