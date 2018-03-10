#include <malloc.h>
#include <pthread.h>
#include "common_cleaner.h"
#include "lists.h"

void free_message(struct msg_list *node) {
    free_vector(&node->msg);
    free(node);
}

void free_thread(struct thread_list* thread) {
    free_client(&thread->client);
    free(thread);
}

void free_all(void *arg) {
    pair_t* p = (pair_t*) arg;
    struct thread_list* thread_head = p->thread_head;
    struct thread_list* cur_thread = thread_head->next;
    free(thread_head);
    while (cur_thread != NULL) {
        struct thread_list* temp = cur_thread->next;
        pthread_cancel(cur_thread->client.writer);
        pthread_cancel(cur_thread->client.reader);
        pthread_join(cur_thread->client.writer, NULL);
        pthread_join(cur_thread->client.reader, NULL);
        free_thread(cur_thread);
        cur_thread = temp;
    }
    struct msg_list* msg_head = p->msg_head;
    struct msg_list* cur_message = msg_head->next;
    free(msg_head);
    while (cur_message != NULL) {
        struct msg_list* temp = cur_message->next;
        free_message(cur_message);
        cur_message = temp;
    }
}
