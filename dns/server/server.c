#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include "../dns.h"

void setup_connection(int* socket_file_descriptor, struct sockaddr_in* server_address) {
    *socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    memset((char*) server_address, 0, sizeof(*server_address));
    server_address -> sin_family = AF_INET;
    server_address -> sin_addr.s_addr = htonl(INADDR_ANY);
    server_address -> sin_port = htons(53333);
    int code = bind(*socket_file_descriptor, (struct sockaddr *) server_address, sizeof(*server_address));
}

ssize_t read_dns_query(char* buffer, int socket_file_descriptor, struct sockaddr_in* client_address) {
    socklen_t sizeof_client_address = sizeof(*client_address);
    return recvfrom(socket_file_descriptor, buffer, MAX_DNS_MESSAGE_LENGTH, 0, (struct sockaddr *) client_address,
                         &sizeof_client_address);
}

void read_domain(char* buffer, char* domain) {
    char* domain_pos = domain;
    char* buffer_pos = buffer;
    size_t block_len = (size_t) *buffer_pos;
    buffer_pos++;
    while (1) {
        memcpy(domain_pos, buffer_pos, block_len);
        buffer_pos += block_len;
        domain_pos += block_len;
        block_len = (size_t) *buffer_pos;
        if (block_len == 0) {
            break;
        }
        buffer_pos++;
        *domain_pos = '.';
        domain_pos++;
    }
}

uint32_t resolve(char* domain) {
    size_t domain_len = strlen(domain);
    uint32_t result = 0;
    for (int i = 0; i < domain_len; i++) {
        result = domain[i] + 31 * result;
    }
    return result;
}

void build_dns_answer(struct dns_answer* answer, const uint32_t* ip) {
    answer -> name = 0xc00c;
    answer -> type = A;
    answer -> class = IN;
    answer -> ttl = 300;
    answer -> rdlength = 4;
    answer -> rddata = (uint8_t*) ip;
}

size_t write_dns_response(char* buffer, ssize_t len) {
    write16(buffer + 2, RESPONSE_FLAG);
    write16(buffer + 6, 1);
    char domain[len];
    memset(domain, 0, sizeof(domain));
    read_domain(buffer + 12, domain);
    uint32_t ip = resolve(domain);
    struct dns_answer answer;
    build_dns_answer(&answer, &ip);
    char* pos = buffer + len;
    pos += write16(pos, answer.name);
    pos += write16(pos, answer.type);
    pos += write16(pos, answer.class);
    pos += write32(pos, answer.ttl);
    pos += write16(pos, answer.rdlength);
    for (int i = 0; i < answer.rdlength; i++) {
        *pos = answer.rddata[i];
        pos++;
    }
    return (size_t) (len + 16);
}

int main() {
    int socket_file_descriptor;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    setup_connection(&socket_file_descriptor, &server_address);
    char buffer[MAX_DNS_MESSAGE_LENGTH];
    while (1) {
        ssize_t query_len = read_dns_query(buffer, socket_file_descriptor, &client_address);
        size_t response_len = write_dns_response(buffer, query_len);
        sendto(socket_file_descriptor, buffer, response_len, 0,(struct sockaddr *) &client_address,
               sizeof(client_address));
    }
}