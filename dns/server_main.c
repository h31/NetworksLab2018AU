#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "dns_header.h"

u_int64_t hash_domain(const char *str) {
    u_int64_t hash = 5381;
    u_int8_t c;
    while ((c = (u_int8_t) *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void server_loop(int socket_id) {
    char buf[PACKET_SIZE], resp[PACKET_SIZE], temp[DOMAIN_BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    while (1) {
        long received = recvfrom(
            socket_id, buf, PACKET_SIZE, 0, (struct sockaddr *) &client_addr, &client_len);
        if (received < 0) {
            perror("ERROR on receiving a datagram");
            exit(1);
        }
        struct dns_header *header = parse_dns_header(buf);

        struct dns_header *resp_header = calloc(1, sizeof(struct dns_header));
        resp_header->id = header->id;
        resp_header->flags = (1 << 15) + (1 << 8) + (1 << 7);
        char *buf_ptr = buf + sizeof(struct dns_header);
        char *resp_ptr = resp + sizeof(struct dns_header);
        for (int i = 0; i < header->qdcount; i++) {
            size_t bytes_read = read_domain_name(buf_ptr, buf, temp);
            buf_ptr += bytes_read;
            int qtype = ntohs(*((u_int16_t*) buf_ptr));
            buf_ptr += 2;
            int qclass = ntohs(*((u_int16_t*) buf_ptr));
            buf_ptr += 2;
            if (qtype != 1 || qclass != 1) {
                // RCODE = Not Implemented
                resp_header->flags |= 4;
                break;
            }
            resp_ptr += append_hostname_to_buffer(temp, resp_ptr);
        }
        size_t resp_len;
        if ((resp_header->flags & 4) == 4) {
            output_dns_header(resp_header, resp);
            resp_len = sizeof(struct dns_header);
        } else {
            resp_header->qdcount = header->qdcount;
            resp_header->ancount = resp_header->qdcount;

            buf_ptr = buf + sizeof(struct dns_header);
            for (int i = 0; i < header->qdcount; i++) {
                size_t bytes_read = read_domain_name(buf_ptr, buf, temp);
                buf_ptr += bytes_read;
                int qtype = ntohs(*((u_int16_t*) buf_ptr));
                buf_ptr += 2;
                int qclass = ntohs(*((u_int16_t*) buf_ptr));
                buf_ptr += 2;
                if (qtype != 1 || qclass != 1) {
                    // RCODE = Not Implemented
                    printf("NOT IMPLEMENTED (id: %d)\n", header->id);
                    resp_header->flags |= 4;
                    break;
                }
                resp_ptr += append_hostname_to_buffer(temp, resp_ptr);
                // append TTL=256
                resp_ptr[0] = 0;
                resp_ptr[1] = 0;
                resp_ptr[2] = 1;
                resp_ptr[3] = 0;
                resp_ptr += 4;
                // append RDLENGTH=4
                resp_ptr[0] = 0;
                resp_ptr[1] = 4;
                resp_ptr += 2;
                // append RDATA
                *((u_int64_t *) resp_ptr) = htonl(hash_domain(temp));
                resp_ptr += 4;
            }

            output_dns_header(resp_header, resp);
            resp_len = resp_ptr - resp;
            printf("ANSWERED (id: %d)\n", header->id);
        }

        free(resp_header);
        sendto(socket_id, resp, resp_len, 0, (struct sockaddr *) &client_addr, client_len);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(0);
    }
    uint16_t port_number = (uint16_t) atoi(argv[1]);

    int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_id < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    /* Now bind the host address using bind() call.*/
    if (bind(socket_id, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    printf("server has started!\n");
    server_loop(socket_id);
    if (close(socket_id) < 0) {
        perror("ERROR on closing the socket");
        exit(1);
    }
    return 0;
}
