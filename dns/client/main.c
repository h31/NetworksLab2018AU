#include <arpa/inet.h> 
#include "../commons/dns.h"

void build_dns_query(struct Dns_query* dns_query, char *domen) {
    dns_query->header->id = 0x0000;
    dns_query->header->flags = 0x0100;
    dns_query->header->questions = 0x0001;
    dns_query->header->answer_RRs = 0x0000;
    dns_query->header->autoritity_RRs = 0x0000;
    dns_query->header->additional_RRs = 0x0000;
    dns_query->queries->domen = domen;
    dns_query->queries->type = A;
    dns_query->queries->class = IN;
}

int write_domen_to_buffer(char *buffer, char *domen) {
    int len = strlen(domen);
    int buffer_pos = 1;
    int prev_dot = -1;
    for (int domen_pos = 0; domen_pos <= len; domen_pos++) {
        if (domen_pos == len || domen[domen_pos] == '.') {
            int word_len = domen_pos - 1 - prev_dot;
            buffer[buffer_pos - word_len - 1] = word_len;
            buffer_pos++;
            prev_dot = domen_pos;
        } else {
            buffer[buffer_pos++] = domen[domen_pos];
        }
    }
    buffer[buffer_pos - 1] = 0;
    return buffer_pos;
}


int write_dns_query_to_buffer(struct Dns_query* dns_query, char *buffer) {
    write_uint16_t_to_buffer(buffer, dns_query->header->id);
    write_uint16_t_to_buffer(buffer + 2, dns_query->header->flags);
    write_uint16_t_to_buffer(buffer + 4, dns_query->header->questions);
    write_uint16_t_to_buffer(buffer + 6, dns_query->header->answer_RRs);
    write_uint16_t_to_buffer(buffer + 8, dns_query->header->autoritity_RRs);
    write_uint16_t_to_buffer(buffer + 10, dns_query->header->additional_RRs);
    int domen_record_len = write_domen_to_buffer(buffer + 12, dns_query->queries->domen);
    write_uint16_t_to_buffer(buffer + 12 + domen_record_len, dns_query->queries->type);
    write_uint16_t_to_buffer(buffer + 14 + domen_record_len, dns_query->queries->class);
    return 16 + domen_record_len;
}

int read_dns_header_from_buffer(struct Dns_header *dns_header, char *buffer) {
    dns_header->id = read_uint16_t_from_buffer(buffer);
    dns_header->flags = read_uint16_t_from_buffer(buffer + 2);
    dns_header->questions = read_uint16_t_from_buffer(buffer + 4);
    dns_header->answer_RRs = read_uint16_t_from_buffer(buffer + 6);
    dns_header->autoritity_RRs = read_uint16_t_from_buffer(buffer + 8);
    dns_header->additional_RRs = read_uint16_t_from_buffer(buffer + 10);
    return 12;
}


int read_domen(char *domen, char *buffer, char *init_buffer) {
    uint8_t mask = (1 << 7) + (1 << 6);
    if ((buffer[0] & mask) == mask) { // starts from 11
        read_row_domen(domen, init_buffer + (read_uint16_t_from_buffer(buffer) - (mask << 8)));
        return 2;
    } else {
        return read_row_domen(domen, buffer);
    }
}

  char *name;
    enum Type type;
    enum Class class;
    uint32_t ttl;
    uint16_t data_len;
    struct IP *ip;

void read_dns_answer_from_buffer(struct Dns_answer *dns_answer, char *buffer, char *init_buffer) {
    int n = read_domen(dns_answer->name, buffer, init_buffer);
    dns_answer->type = read_uint16_t_from_buffer(buffer + n);
    dns_answer->class = read_uint16_t_from_buffer(buffer + n + 2);
    dns_answer->ttl = read_uint32_t_from_buffer(buffer + n + 4);
    dns_answer->data_len = read_uint16_t_from_buffer(buffer + n + 8);
    dns_answer->ip->k1 = buffer[n + 10];
    dns_answer->ip->k2 = buffer[n + 11];
    dns_answer->ip->k3 = buffer[n + 12];
    dns_answer->ip->k4 = buffer[n + 13];
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "usage %s dns-server domen port?\n", argv[0]);
        exit(0);
    }
    uint16_t portno = (argc == 3 ? DNS_PORT : atoi(argv[3]));

    char *dns_server = argv[1];
    char *domen = argv[2];
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    socklen_t serv_addr_sz = sizeof(serv_addr);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(dns_server);
    serv_addr.sin_port = htons(portno);

    struct Dns_query dns_query;
    struct Dns_header dns_header;
    struct Dns_queries dns_queries;
    dns_query.header = &dns_header;
    dns_query.queries = &dns_queries;
    char buffer[MAX_MSG_LEN];
    bzero(buffer, MAX_MSG_LEN);
    build_dns_query(&dns_query, domen);
    int query_len = write_dns_query_to_buffer(&dns_query, buffer);
    int sent = sendto(sockfd, buffer, query_len, 0, (struct sockaddr *) &serv_addr, serv_addr_sz);
    if (sent < 0) {
        perror("ERROR on send");
        close(sockfd);
        exit(1);
    }
    bzero(buffer, MAX_MSG_LEN);
    recvfrom(sockfd, buffer, MAX_MSG_LEN, 0, (struct sockaddr *) &serv_addr, &serv_addr_sz);

    struct Dns_response dns_response;
    struct Dns_answer dns_answer;
    struct IP ip;
    dns_response.header = &dns_header;
    dns_response.queries = &dns_queries;
    dns_response.answer = &dns_answer;
    dns_answer.ip = &ip;
    char answer_name[MAX_MSG_LEN];
    dns_answer.name = answer_name;

    read_dns_answer_from_buffer(dns_response.answer, buffer + query_len, buffer);

    printf("name: %s\n", dns_response.answer->name);
    printf("type: %d\n", dns_response.answer->type);
    printf("class: %d\n", dns_response.answer->class);
    printf("ttl: %d\n", dns_response.answer->ttl);
    printf("data_len: %d\n", dns_response.answer->data_len);
    printf("IP: %d.%d.%d.%d\n", dns_response.answer->ip->k1, dns_response.answer->ip->k2, dns_response.answer->ip->k3, dns_response.answer->ip->k4);

    close(sockfd);
    return 0;
}