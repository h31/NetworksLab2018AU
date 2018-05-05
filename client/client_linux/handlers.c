#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <client.h>
#include "vector.h"
#include "client.h"
#include "common_cleaner.h"

typedef enum {READ, WRITE} client_mode;
client_mode mode = WRITE;

void read_line(vector_t* msg) {
    char ch;
    msg->size = 0;
    msg->capacity = 256;
    msg->data = malloc(sizeof(char) * msg->capacity);
    bzero(msg->data, msg->capacity);
    scanf("%c", &ch);
    while (ch != '\n') {
        append(msg, ch);
        scanf("%c", &ch);
    }
}

void* reader(void *arg) {
    client_t* client = (client_t*) arg;
    int sockfd = client->sockfd;
    while (1) {
        vector_t msg = {};
        int n = read_message(sockfd, &msg);
        pthread_cleanup_push(free_vector, &msg);
                if (n < 0) {
                    pthread_mutex_lock(&client->mutex);
                    client->is_closed = true;
                    pthread_mutex_unlock(&client->mutex);
                    pthread_exit(NULL);
                }
                pthread_mutex_lock(&client->mutex);
                pthread_cleanup_push(mutex_unlock, &client->mutex);
                        while (mode != READ && client->is_closed == false) {
                            pthread_cond_wait(&client->can_consume, &client->mutex);
                        }
                        if (client->is_closed == true) {
                            pthread_exit(NULL);
                        }
                        printf("%s\n", msg.data);
                pthread_cleanup_pop(1);
        pthread_cleanup_pop(1);
    }
}


void* writer(void *arg) {
    client_t* client = (client_t*) arg;
    vector_t name = client->name;
    pthread_cleanup_push(close_socket, &client->sockfd);

            int sockfd = client->sockfd;
            pthread_mutex_lock(&client->mutex);
            pthread_cleanup_push(mutex_unlock, &client->mutex);
                    ssize_t n = write_message(sockfd, &name);
                    if (n < 0) {
                        perror("ERROR: cannot write to socket");
                        client->is_closed = true;
                        pthread_cond_signal(&client->can_consume);
                        pthread_exit(NULL);
                    }
                    mode = READ;
                    pthread_cond_signal(&client->can_consume);
            pthread_cleanup_pop(1);

            while (1) {
                char m = '\0';
                while (m != 'm') {
                    scanf("%c", &m);
                }
                getchar();
                pthread_mutex_lock(&client->mutex);
                pthread_cleanup_push(mutex_unlock, &client->mutex);
                        mode = WRITE;
                        printf("[%s]: ", name.data);
                        vector_t msg = {};
                        pthread_cleanup_push(free_vector, &msg);
                                read_line(&msg);
                                mode = READ;
                                ssize_t n = write_message(sockfd, &msg);
                                if (n < 0) {
                                    client->is_closed = true;
                                    pthread_cond_signal(&client->can_consume);
                                    pthread_exit(NULL);
                                }
                        pthread_cleanup_pop(1);
                        pthread_cond_signal(&client->can_consume);
                        pthread_mutex_unlock(&client->mutex);
                pthread_cleanup_pop(0);
            }
    pthread_cleanup_pop(1);
}
