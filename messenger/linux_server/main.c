#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

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

	/* Accept actual connection from the client */
	int client_socket = accept(server_socket, (struct sockaddr *) &cli_addr, &clilen);

	if (client_socket < 0) {
		perror("ERROR on accept");
		exit(1);
	}

	return client_socket;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
		fprintf(stderr, "usage %s port\n", argv[0]);
		exit(0);
	}

    int server_socket = create_server_socket((uint16_t) atoi(argv[1]));
    int client_socket = accept_client(server_socket);

    char *string = receive_cstring(client_socket);
    printf("%s\n", string);

    free(string);

    close(client_socket);
    close(server_socket);

    return 0;
}

