#include <stdio.h>
#include <string.h>

#include "clients.h"
#include "../common/communication.h"

void* client_interaction_routine(void* arg) {
	struct client_data *client_data = (struct client_data*)arg;

	char *buffer = malloc(sizeof(char) * (MAX_MSG_LEN + 1));
	bzero(buffer, sizeof(char) * (MAX_MSG_LEN + 1));

	while (receive_cstring(client_data->sock, buffer) > 0) {
		printf("%s\n", buffer);
		broadcast_message(buffer, client_data->broadcast_mutex);
	}

	free(buffer);
	close(client_data->sock);
	client_data->is_valid = 0;

	return NULL;
}

struct client_data* find_empty_client_cell() {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (!clients[i].is_valid) {
			pthread_join(clients[i].thread, NULL);
			return clients + i;
		}
	}

	return NULL;
}

void broadcast_message(char *message, pthread_mutex_t *broadcast_mutex) {
	pthread_mutex_lock(broadcast_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (!clients[i].is_valid) {
			continue;
		}

		if (send_cstring(clients[i].sock, message) < 0) {
			fprintf(stderr, "Failed to send data to socket #%d\n", clients[i].sock);
		}
	}

	pthread_mutex_unlock(broadcast_mutex);
}
