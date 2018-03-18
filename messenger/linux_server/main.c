#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>

#define MAX_COUNT_CLIENTS 100

int clients[MAX_COUNT_CLIENTS];
char is_active[MAX_COUNT_CLIENTS];
int count_active_clients;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static inline int reserve_socket_cell() {
    for (size_t i = 0; i < sizeof(is_active) / sizeof(is_active[0]); i++) {
        if (is_active[i] == 0) {
            pthread_mutex_lock(&mtx);
            ++count_active_clients;
            is_active[i] = 1;
            pthread_mutex_unlock(&mtx);
            return i;
        }
    }
    return -1;
}

static inline void free_socket_cell(int cell) {
    pthread_mutex_lock(&mtx);
    is_active[cell] = 0;
    --count_active_clients;
    pthread_mutex_unlock(&mtx);
}

static inline void notify_all(char *buffer, char message_len, int skip) {
    for (int i = 0; i < MAX_COUNT_CLIENTS; i++) {
        if (i == skip || is_active[i] == 0)
            continue;

        int n = 1;
        while (n != 0) {
            int r = write(clients[i], &message_len, n);

            if (r < 0) {
                continue;
            }

            n -= r;
        } 

        n = message_len;

        while (n != 0) {
            int r = write(clients[i], buffer + message_len - n, n);

            if (r < 0) {
                continue;
            }

            n -= r;
        }
    }
}

static void* client_handler(void * arg) {
    int cell = (char*)arg - is_active;
    char buffer[256];

    while (1) {
        char message_len = 0;
        int n = read(clients[cell], &message_len, 1); // recv on Windows

        if (n == 0) {
            continue;
        }

        if (n < 0) {
            perror("ERROR reading from socket");
            break;
        }

        bzero(buffer, 256);
        
        char size = message_len;
        while (size != 0) {
            n = read(clients[cell], buffer + message_len - size, size); // recv on Windows

            if (n < 0) {
                perror("ERROR reading from socket");
                break;
            }

            size -= n;
        }

        printf("Message get: %s\n", buffer);

        notify_all(buffer, message_len, cell);
    }

    free_socket_cell(cell);

    return NULL;
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    uint16_t portno;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    (void)argc;
    (void)argv;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        return 1;
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        close(sockfd);
        return 1;
    }

    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    /* Accept actual connection from the client */
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }

        if (count_active_clients + 1 > MAX_COUNT_CLIENTS) {
            perror("Customer limit exceeded");
            close(newsockfd);
            continue;
        }

        int cell = reserve_socket_cell();

        if (cell == -1) {
            perror("Customer limit exceeded");
            close(newsockfd);
            continue;
        }

        clients[cell] = newsockfd;

        pthread_t thread_id;

        if (pthread_create(&thread_id, NULL, client_handler, is_active + cell) != 0) {
            continue;
        }

        pthread_detach(thread_id);
    }

    return 0;
}
