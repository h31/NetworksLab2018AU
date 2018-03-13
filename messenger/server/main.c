#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "../common/network.h"
#include "../common/communication.h"
#include "../common/threads.h"

#include "clients.h"

int create_server_socket(uint16_t portno) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
        perror("ERROR opening socket");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));

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

void run_server(int server_socket, lock_t *broadcast_lock) {
	// todo: add proper shutdown
	while (1) {
		int client_socket = accept_client(server_socket);

		struct client_data *client_data = find_empty_client_cell();
		if (client_data == NULL) {
			close(client_socket);
			continue;
		}

		client_data->state = INITIALIZED;
		client_data->sock = client_socket;
		client_data->broadcast_lock = broadcast_lock;

		client_data->thread = thread_create(client_interaction_routine, client_data);
	}
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
		fprintf(stderr, "usage %s port\n", argv[0]);
		exit(0);
	}

    network_init();

    lock_t *broadcast_lock = lock_create();

    int server_socket = create_server_socket((uint16_t) atoi(argv[1]));

    run_server(server_socket, broadcast_lock);

    close(server_socket);
    lock_destroy(broadcast_lock);

    return 0;
}

