#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

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

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    return sockfd;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage %s hostname port nickname\n", argv[0]);
        exit(0);
    }

    int sockfd = connect_to_server(argv[1], (uint16_t) atoi(argv[2]));
    send_cstring(sockfd, "Hello.");

    close(sockfd);
    return 0;
}

