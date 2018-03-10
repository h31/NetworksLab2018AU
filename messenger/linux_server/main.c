#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#include <pthread.h>

#include "../common/communication.h"

int create_server_socket(uint16_t portno) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	listen(sockfd, 5);

	return sockfd;
}

int accept_client(int server_socket) {
	struct sockaddr_in cli_addr;
	unsigned int clilen = sizeof(cli_addr);

	int client_socket = accept(server_socket, (struct sockaddr *) &cli_addr, &clilen);

	if (client_socket < 0) {
		perror("ERROR on accept");
		exit(1);
	}

	return client_socket;
}

typedef struct client_data {
	uint8_t is_valid;
	int sock;
	pthread_t thread;
} client_data_t;

void* client_interaction_routine(void* arg) {
	client_data_t *client_data = (client_data_t*)arg;

	char *buffer = malloc(MAX_MSG_LEN + 1);
	bzero(buffer, sizeof(char) * (MAX_MSG_LEN + 1));

	while (receive_cstring(client_data->sock, buffer) != 0) {
		printf("%s\n", buffer);
	}

	free(buffer);
	close(client_data->sock);

	return NULL;
}

client_data_t* find_empty_client_cell(client_data_t *clients, size_t size) {
	for (int i = 0; i < size; ++i) { // todo: add thread check (it should be communication)
		if (!clients[i].is_valid) {
			return clients + i;
		}
	}

	return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
		fprintf(stderr, "usage %s port\n", argv[0]);
		exit(0);
	}

    int server_socket = create_server_socket((uint16_t) atoi(argv[1]));

    int max_clients = 100;
    client_data_t clients[max_clients];
    bzero(clients, sizeof(clients));

    while (1) {
    	int client_socket = accept_client(server_socket);

    	client_data_t *client_data = find_empty_client_cell(clients, max_clients);
    	if (client_data == NULL) {
    		continue;
    	}

    	client_data->is_valid = 1;
    	client_data->sock = client_socket;

    	pthread_create(&client_data->thread, NULL, client_interaction_routine, client_data);
    }

    close(server_socket);

    return 0;
}

