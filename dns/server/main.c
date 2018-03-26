#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dns_protocol.h"

#define BUFFER_SIZE 65536
static const unsigned short PORT = 53;

static void assert_error(char expression, const char *msg)
{
    if (!expression)
    {
        perror(msg);
        exit(1);
    }
}

int main()
{
    const int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);  
    assert_error(server_socket_fd >= 0, "ERROR: opening socket");

    const int optval = 1; 
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)); 

    struct sockaddr_in serveraddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    assert_error(bind(server_socket_fd, (struct sockaddr *)&serveraddr, sizeof serveraddr) >= 0, "ERROR: bind");  

    while (1)
    {
        struct sockaddr_in clientaddr = { };
        unsigned int clientlen = sizeof clientaddr;
        unsigned char buffer[BUFFER_SIZE] = { };
        printf("Receiving request...\n");
        int n = recvfrom(server_socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
        printf("Done\n");
        assert_error(n >= 0, "ERROR: recvfrom");
        struct DNS_HEADER *dns = (struct DNS_HEADER*)buffer;
        unsigned char *qname =    (unsigned char*)(buffer + sizeof(struct DNS_HEADER));
        dns->qr = 1;
        dns->ans_count = htons(1);
        unsigned char* reader = buffer + sizeof(struct DNS_HEADER) + strlen((const char*)qname) + 1 + sizeof(struct QUESTION);
        memcpy(reader, qname, strlen((char*)qname) + 1);
        reader += strlen((char*)qname) + 1;

        struct R_DATA* data = (struct R_DATA*)reader;
        data->type = htons(T_A);
        data->ttl = htonl(200);
        data->_class = htons(1);
        data->data_len = htons(4);
        reader += sizeof(struct R_DATA);
        *(int*)reader = htonl(strlen((char*)qname));
        n += strlen((char*)qname) + 1 + sizeof(struct R_DATA) + 4;

        const struct hostent * const hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof clientaddr.sin_addr.s_addr, AF_INET);
        assert_error(hostp != NULL, "ERROR: gethostbyaddr");
        const char * const hostaddrp = inet_ntoa(clientaddr.sin_addr);
        assert_error(hostaddrp != NULL, "ERROR: inet_ntoa");
        printf("Sending packet...\n");
        n = sendto(server_socket_fd, buffer, n, 0, (struct sockaddr *) &clientaddr, clientlen);
        printf("Done\n");
        assert_error(n >= 0, "ERROR: sendto");
    }

    return 0;
}
