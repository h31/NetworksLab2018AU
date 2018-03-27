#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "dns_header.h"

void interpret_opcode(int opcode) {
    switch (opcode) {
        case 0:
            printf("No error condition\n");
            break;
        case 1:
            printf("Format error\n");
            exit(0);
        case 2:
            printf("Server failure\n");
            exit(0);
        case 3:
            printf("Name Error\n");
            exit(0);
        case 4:
            printf("Not Implemented\n");
            exit(0);
        case 5:
            printf("Refused\n");
            exit(0);
        default:
            printf("Unknown opcode = %d\n", opcode);
            exit(0);
    }
}

void process_data(const char *buf) {
    struct dns_header* header = parse_dns_header(buf);
    const char *ptr = buf + sizeof(struct dns_header);
    interpret_opcode(header->flags & 15);
    char temp[DOMAIN_BUFFER_SIZE];
    for (int query_id = 0; query_id < header->qdcount; query_id++) {
        ptr += read_domain_name(ptr, buf, temp);
        // skip QTYPE(2) & QCLASS(2)
        ptr += 4;
    }
    for (int answer_id = 0; answer_id < header->ancount; answer_id++) {
        size_t bytes_read = read_domain_name(ptr, buf, temp);
        puts(temp);
        ptr += bytes_read;
        int type = ntohs(*((u_int16_t *) ptr));
        if (type != 1) {
            printf("Unsupported type in answer %d\n", type);
            exit(0);
        }
        // skip TYPE(2) & CLASS(2) & TTL(4) & RDLENGTH(2)
        ptr += 10;
        for (int i = 0; i < 4; i++) {
            printf("%d", (u_int8_t) *ptr);
            ptr += 1;
            if (i != 3) {
                putchar('.');
            } else {
                putchar('\n');
            }
        }
    }
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));
    if (argc < 4) {
        fprintf(stderr, "usage %s ip_address port hostname\n", argv[0]);
        exit(0);
    }
    uint16_t port_number = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    int socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_id < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port_number);

    char buf[PACKET_SIZE];
    memset(buf, 0, sizeof(buf));
    struct dns_header *header = (struct dns_header *) buf;
    header->id = (u_int16_t) rand();
    header->flags = (1 << 8);
    header->qdcount = 1;
    size_t buf_len = output_dns_header(header, buf);
    buf_len += append_hostname_to_buffer(argv[3], buf + buf_len);
    sendto(socket_id, buf, buf_len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (recv(socket_id, buf, 255, 0) < 0) {
        perror("ERROR receiving data");
        exit(1);
    }
    process_data(buf);
    if (close(socket_id) < 0) {
        perror("ERROR on closing the socket");
        exit(1);
    }
    return 0;
}
