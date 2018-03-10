#ifndef CLIENTS_H_
#define CLIENTS_H_

#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>

struct client_data {
	uint8_t is_valid;
	int sock;
	pthread_t thread;

	pthread_mutex_t *broadcast_mutex;
};

#define MAX_CLIENTS (100)

struct client_data clients[MAX_CLIENTS];

void* client_interaction_routine(void* arg);
struct client_data* find_empty_client_cell();
void broadcast_message(char *message, pthread_mutex_t *broadcast_mutex);

#endif /* CLIENTS_H_ */
