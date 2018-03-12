#include <stdio.h>
#include <string.h>
#include <time.h>

#include "clients.h"
#include "../common/communication.h"

void* client_interaction_routine(void* arg) {
	struct client_data *client_data = (struct client_data*)arg;

	char *buffer = malloc(sizeof(char) * (MAX_CHUNK_LEN + 1));
	memset(buffer, 0, sizeof(char) * (MAX_CHUNK_LEN + 1));

	if (receive_cstring(client_data->sock, buffer) > 0) {
		client_data->nickname = malloc(strlen(buffer) + 1);
		strcpy(client_data->nickname, buffer);

		while (receive_cstring(client_data->sock, buffer) > 0) {
			time_t rawtime;
			time(&rawtime);

			struct tm *timeinfo = localtime(&rawtime);

			struct message msg = {timeinfo->tm_hour, timeinfo->tm_min, client_data->nickname, buffer};
			broadcast_message(msg, client_data->broadcast_lock);
		}
	}

	free(buffer);
	free(client_data->nickname);
	close(client_data->sock);
	client_data->state = REQUIRE_DELETION;

	return NULL;
}

struct client_data* find_empty_client_cell() {
	int i;
	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].state != INITIALIZED) {
			if (clients[i].state == REQUIRE_DELETION) {
				thread_join(clients[i].thread);
				clients[i].state = UNITIALIZED;
			}

			return clients + i;
		}
	}

	return NULL;
}

void broadcast_message(struct message msg, lock_t *broadcast_lock) {
	lock(broadcast_lock);

    int i;
	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].state != INITIALIZED) {
			continue;
		}

		if (send_uint16(clients[i].sock, msg.hours) < 0 ||
			send_uint16(clients[i].sock, msg.minutes) < 0 ||
			send_cstring(clients[i].sock, msg.nickname) < 0 ||
			send_cstring(clients[i].sock, msg.text) < 0) {
			fprintf(stderr, "Failed to send data to socket #%d\n", clients[i].sock);
		}
	}

	unlock(broadcast_lock);
}
