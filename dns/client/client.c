#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "../dns.h"

void parse_command_line_arguments(int argc, char* argv[], char** dns_server, char** domain, uint16_t* port) {
    if (argc != 4) {
        fprintf(stderr, "Usage examples (dns-server port domain): 8.8.8.8 53 random.org\n"
                "127.0.0.1 53333 random.org");
        exit(0);
    }
    *dns_server = argv[1];
    *port = (uint16_t) atoi(argv[2]);
    *domain = argv[3];
}

void setup_connection(int *socket_file_descriptor, struct sockaddr_in *server_address, const char *dns_server,
                      uint16_t port) {
    *socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    memset((char*) server_address, 0, sizeof(*server_address));
    server_address -> sin_family = AF_INET;
    server_address -> sin_addr.s_addr = inet_addr(dns_server);
    server_address -> sin_port = htons(port);
}

void build_dns_header(struct dns_header* header) {
    header -> id = 1234;
    header -> flags = REQUEST_FLAG;
    header -> qdcount = 1;
    header -> ancount = 0;
    header -> nscount = 0;
    header -> arcount = 0;
}

void build_dns_question(struct dns_question* question, char* domain) {
    question -> qname = domain;
    question -> type = A;
    question -> class = IN;
}

int writestr(char* buffer, char* domain) {
    uint16_t l = 0;
    uint16_t r = 0;
    size_t n = strlen(domain);
    char* old_pos = buffer;
    char* pos = buffer;
    while (l < n) {
        while (r < n && domain[r] != '.') {
            r++;
        }
        size_t m = r - l;
        *pos = (char) m;
        pos++;
        memcpy(pos, domain + l, m);
        pos += m;
        r++;
        l = r;
    }
    pos++;
    return (int) (pos - old_pos);
}

ssize_t send_dns_query(char* buffer, struct dns_header *header, struct dns_question *question, int socket_file_descriptor,
                       struct sockaddr_in* server_address) {
    memset(buffer, 0, sizeof(buffer));
    char* old_pos = buffer;
    char* pos = buffer;
    pos += write16(pos, header -> id);
    pos += write16(pos, header -> flags);
    pos += write16(pos, header -> qdcount);
    pos += write16(pos, header -> ancount);
    pos += write16(pos, header -> nscount);
    pos += write16(pos, header -> arcount);
    pos += writestr(pos, question -> qname);
    pos += write16(pos, question -> type);
    pos += write16(pos, question -> class);
    size_t len = pos - old_pos;
    ssize_t sent = sendto(socket_file_descriptor, buffer, len, 0, (struct sockaddr*) server_address,
                          sizeof(*server_address));
    return sent;
}

int main(int argc, char* argv[]) {
    char* dns_server;
    char* domain;
    uint16_t port;
    parse_command_line_arguments(argc, argv, &dns_server, &domain, &port);
    int socket_file_descriptor;
    struct sockaddr_in server_address;
    setup_connection(&socket_file_descriptor, &server_address, dns_server, port);
    struct dns_header header;
    build_dns_header(&header);
    struct dns_question question;
    build_dns_question(&question, domain);

    char buffer[MAX_DNS_MESSAGE_LENGTH];
    ssize_t len = send_dns_query(buffer, &header, &question, socket_file_descriptor, &server_address);
    socklen_t sizeof_server_address = sizeof(server_address);
    memset(buffer, 0, sizeof(buffer));
    recvfrom(socket_file_descriptor, buffer, MAX_DNS_MESSAGE_LENGTH, 0, (struct sockaddr*) &server_address,
             &sizeof_server_address);
    uint16_t answers = read16(buffer + 6);
    char* pos = buffer + len;
    for (int i = 0; i < answers; i++) {
        pos += 12;
        for (int j = 0; j < 4; j++) {
            printf("%d", (*pos) &0xff);
            if (j != 3)
                printf(".");
            pos++;
        }
        printf("\n");
    }
    close(socket_file_descriptor);
    return 0;
}

