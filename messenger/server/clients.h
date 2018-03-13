#ifndef CLIENTS_H_
#define CLIENTS_H_

#include <stdlib.h>
#include <stdint.h>

#include "../common/threads.h"

enum client_data_state {UNITIALIZED = 0, INITIALIZED, REQUIRE_DELETION};

struct client_data {
	enum client_data_state state;
	int sock;
	thread_t *thread;
	char *nickname;

	lock_t *broadcast_lock;
};

struct message {
	uint16_t hours;
	uint16_t minutes;
	char *nickname;
	char *text;
};

#define MAX_CLIENTS (100)

struct client_data clients[MAX_CLIENTS];

void* client_interaction_routine(void* arg);
struct client_data* find_empty_client_cell();
void broadcast_message(struct message msg, lock_t *broadcast_mutex);

#endif /* CLIENTS_H_ */
