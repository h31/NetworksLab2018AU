#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <pthread.h>
#include "lists.h"
#include "client.h"
#include "common_cleaner.h"

// msg_mutex and condition variable for message list
pthread_cond_t msg_can_consume = PTHREAD_COND_INITIALIZER;
pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;

struct msg_list* msg_tail;
struct msg_list* msg_head;

void free_all(void *arg);

void* handle_client_read(void* arg) {
    client_t* client = (client_t*) arg;
    pthread_cleanup_push(thread_cancel, &client->writer);
            // writer closed socket

            // read nick name and signal writer
            pthread_mutex_lock(&client->mutex);
            pthread_cleanup_push(mutex_unlock, &client->mutex);
                    int n = read_message(client->sockfd, &client->name);
                    pthread_cond_signal(&client->can_consume);
                    pthread_mutex_unlock(&client->mutex);
                    if (n < 0) {
                        perror("cannot read nick name of client from socket");
                        pthread_exit(NULL);
                    }

                    while (1) {
                        vector_t read_msg = {};
                        pthread_cleanup_push(free_vector, &read_msg);
                                n = read_message(client->sockfd, &read_msg);
                                if (n < 0) {
                                    pthread_exit(NULL);
                                }
                        pthread_cleanup_pop(0);
                        struct msg_list* node = malloc(sizeof(struct msg_list));
                        node->next = NULL;
                        node->client_name = client->name;
                        vector_t msg;
                        msg.size = read_msg.size + client->name.size + 5;
                        msg.capacity = msg.size + 1;
                        msg.data = malloc(sizeof(char) * msg.capacity);
                        snprintf(msg.data, msg.size, "[%s]: %s", client->name.data,
                                 read_msg.data);
                        msg.data[msg.size] = '\0';
                        free_vector(&read_msg);
                        node->msg = msg;

                        // add message to list and signal all writers
                        pthread_mutex_lock(&msg_mutex);
                        msg_tail->next = node;
                        msg_tail = node;
                        pthread_cond_broadcast(&msg_can_consume);
                        pthread_mutex_unlock(&msg_mutex);
                        pthread_testcancel();
                    }
            pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);
}

void* handle_client_write(void* arg) {
    client_t* client = (client_t*) arg;
    pthread_cleanup_push(thread_cancel, &client->reader);
            pthread_cleanup_push(close_socket, &client->sockfd);

                    // wait untill read the nick name
                    pthread_mutex_lock(&client->mutex);
                    while (client->name.data == NULL) {
                        pthread_cond_wait(&client->can_consume, &client->mutex);
                    }

                    struct msg_list* cur = msg_head;
                    while (1) {
                        pthread_mutex_lock(&msg_mutex);
                        pthread_cleanup_push(mutex_unlock, &msg_mutex);
                                while (cur->next == NULL) {
                                    pthread_cond_wait(&msg_can_consume, &msg_mutex);
                                }
                                cur = cur->next;
                                if (strcmp(client->name.data, cur->client_name.data) != 0) {
                                    ssize_t n = write_message(client->sockfd, &cur->msg);
                                    if (n < 0) {
                                        perror("ERROR: cannot write to client");
                                        pthread_exit(NULL);
                                    }
                                }
                        pthread_cleanup_pop(1);
                        pthread_testcancel();
                    }
            pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return NULL;
}

void* server(void* arg) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        return NULL;
    }

    struct sockaddr_in serv_addr;

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    uint16_t portno = *(uint16_t*) arg;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        return NULL;
    }

    struct thread_list* thread_head = malloc(sizeof(struct thread_list));
    thread_head->next = NULL;
    msg_head = malloc(sizeof(struct msg_list));
    msg_head->next = NULL;
    msg_tail = msg_head;
    struct thread_list* cur = thread_head;
    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */

    listen(sockfd, 5);
    struct sockaddr_in cli_addr;
    unsigned int clilen = sizeof(cli_addr);

    pthread_cleanup_push(close_socket, &sockfd);
            pair_t p = {.msg_head = msg_head, .thread_head = thread_head};
            pthread_cleanup_push(free_all, &p);
                    /* Accept actual connection from the client */
                    while (1) {
                        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                        if (newsockfd < 0) {
                            perror("ERROR on accept");
                            continue;
                        }
                        struct thread_list* thread = malloc(sizeof(struct thread_list));
                        thread->next = NULL;
                        cur->next = thread;
                        client_t client = {};
                        client.sockfd = newsockfd;
                        pthread_mutex_init(&client.mutex, NULL);
                        pthread_cond_init(&client.can_consume, NULL);
                        thread->client = client;
                        pthread_create(&thread->client.reader, NULL, handle_client_read,
                                       &thread->client);
                        pthread_create(&thread->client.writer, NULL, handle_client_write,
                                       &thread->client);
                        cur = cur->next;
                    }
            pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
}