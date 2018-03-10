#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <pthread.h>
#include <memory.h>
#include "common_cleaner.h"

void get_options(int argc, char** argv, char** server_addr,
                 uint16_t* server_port, char** nick_name);
void* reader(void *arg);
void* writer(void *arg);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    uint16_t portno;
    char* server_addr;
    char* nick_name;
    get_options(argc, argv, &server_addr, &portno, &nick_name);

    /* Create a socket point */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    struct hostent *server = gethostbyname(server_addr);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    /* Now ask for a name from the user, this name
       * will be read by server
    */

    vector_t name;
    name.data = nick_name;
    name.size = strlen(nick_name);
    name.capacity = name.size;
    client_t client;
    client.sockfd = sockfd;
    client.name = name;
    pthread_mutex_init(&client.mutex, NULL);
    pthread_cond_init(&client.can_consume, NULL);
    pthread_create(&client.reader, NULL, reader, &client);
    pthread_create(&client.writer, NULL, writer, &client);

    pthread_join(client.reader, NULL);
    pthread_join(client.writer, NULL);
    free(server_addr);
    free_client(&client);
}