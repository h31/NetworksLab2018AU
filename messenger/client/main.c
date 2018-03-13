#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "../common/network.h"
#include "../common/threads.h"
#include "../common/communication.h"

int connect_to_server(char *hostname, uint16_t portno) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(hostname);

    if (server == NULL) {
    	perror("ERROR, no such host");
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr, (size_t)server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    return sockfd;
}

void* receiver_routine(void* arg) {
	int server_socket = *((int*)arg);

	uint16_t hours, minutes;
	char nickname_buffer[MAX_CHUNK_LEN + 1];
	char text_buffer[MAX_CHUNK_LEN + 1];

	while (1) {
		thread_is_interrupted();

		if (receive_uint16(server_socket, &hours) <= 0 ||
			receive_uint16(server_socket, &minutes) <= 0 ||
			receive_cstring(server_socket, nickname_buffer) <= 0 ||
			receive_cstring(server_socket, text_buffer) <= 0) {
			break;
		}

		printf("[%02d:%02d] %s: %s\n", hours, minutes, nickname_buffer, text_buffer);
	}

	return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage %s host port nickname\n", argv[0]);
        exit(0);
    }

    network_init();

    int server_socket = connect_to_server(argv[1], (uint16_t) atoi(argv[2]));
    char buffer[MAX_CHUNK_LEN + 1];

    thread_t *receiver_thread = thread_create(receiver_routine, &server_socket);

    send_cstring(server_socket, argv[3]);
    while (feof(stdin) || fgets(buffer, MAX_CHUNK_LEN, stdin) != 0) {
    	send_cstring(server_socket, buffer);
    }

    thread_interrupt(receiver_thread);
    thread_join(receiver_thread);
    close(server_socket);

    return 0;
}

