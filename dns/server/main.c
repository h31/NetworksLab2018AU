#include "../commons/dns.h"

int read_dns_query_from_buffer(struct Dns_query *dns_query, char *buffer) {
    bzero(dns_query->queries->domen, MAX_MSG_LEN);
    dns_query->header->id = read_uint16_t_from_buffer(buffer);
    dns_query->header->flags = read_uint16_t_from_buffer(buffer + 2);
    dns_query->header->questions = read_uint16_t_from_buffer(buffer + 4);
    dns_query->header->answer_RRs = read_uint16_t_from_buffer(buffer + 6);
    dns_query->header->autoritity_RRs = read_uint16_t_from_buffer(buffer + 8);
    dns_query->header->additional_RRs = read_uint16_t_from_buffer(buffer + 10);
    int n = read_row_domen(dns_query->queries->domen, buffer + 12);
    dns_query->queries->type = read_uint16_t_from_buffer(buffer + 12 + n);
    dns_query->queries->class = read_uint16_t_from_buffer(buffer + 14 + n);
    return 16 + n;
}

void set_response_flags(char *buffer) {
    write_uint16_t_to_buffer(buffer + 2, 0x8180);
}

void set_answer_number(char *buffer) {
    write_uint16_t_to_buffer(buffer + 6, 1);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(0);
    }
    uint16_t portno = atoi(argv[1]);
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    char buffer[MAX_MSG_LEN];
    char domen[MAX_MSG_LEN];

    struct Dns_query dns_query;
    struct Dns_header dns_header;
    struct Dns_queries dns_queries;
    dns_query.header = &dns_header;
    dns_query.queries = &dns_queries;
    dns_queries.domen = domen;
    while (1) {
        bzero(buffer, MAX_MSG_LEN);
        recvfrom(sockfd, buffer, MAX_MSG_LEN, 0,(struct sockaddr *) &cli_addr, &cli_len);
        set_response_flags(buffer);
        set_answer_number(buffer);
        int query_len = read_dns_query_from_buffer(&dns_query, buffer);
        write_uint16_t_to_buffer(buffer + query_len, (1 << 15) + (1 << 14) + 12);
        write_uint16_t_to_buffer(buffer + query_len + 2, dns_query.queries->type);
        write_uint16_t_to_buffer(buffer + query_len + 4, dns_query.queries->class);
        write_uint32_t_to_buffer(buffer + query_len + 6, TTL);
        write_uint16_t_to_buffer(buffer + query_len + 10, 4);
        // ip is being calculated as four first letters of domen
        for (int i = 0; i < 4; i++) {
            buffer[query_len + 12 + i] = dns_query.queries->domen[i];    
        }
        int n = sendto(sockfd, buffer, query_len + 16, 0,(struct sockaddr *) &cli_addr, cli_len);
        if (n < 0) {
            perror("ERROR on sending");
        }
    }

    close(sockfd);
    return 0;
}
