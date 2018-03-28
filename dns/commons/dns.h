#ifndef __DNS_H__
#define __DNS_H__

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#define DNS_PORT 53
#define MAX_MSG_LEN 256
#define TTL 300


enum Type {
    A = 1,
    NS = 2,
    CNAME = 5,
    SOA = 6,
    MB = 7,
    WKS = 11,
    PRT = 12,
    NINFO = 13,
    MINFO = 14,
    MX = 15,
    TXT = 16,
    AXFR = 252,
    ANY = 255
};

enum Class {
    IN = 1,
    CH = 3,
    HS = 4
};

struct Dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answer_RRs;
    uint16_t autoritity_RRs;
    uint16_t additional_RRs;

};

struct Dns_queries {
    char *domen;
    enum Type type;
    enum Class class;
};

struct Dns_query {
    struct Dns_header *header;
    struct Dns_queries *queries;
};

struct IP {
    uint8_t k1;
    uint8_t k2;
    uint8_t k3;
    uint8_t k4;
};

struct Dns_answer
{
    char *name;
    enum Type type;
    enum Class class;
    uint32_t ttl;
    uint16_t data_len;
    struct IP *ip;
};

struct Dns_response {
    struct Dns_header *header;
    struct Dns_queries *queries;
    struct Dns_answer *answer;
};

int read_row_domen(char *domen, char *buffer);

uint16_t read_uint16_t_from_buffer(char *buffer);

uint32_t read_uint32_t_from_buffer(char *buffer);

void write_uint16_t_to_buffer(char *buffer, uint16_t value);

void write_uint32_t_to_buffer(char *buffer, uint32_t value);

#endif // __DNS_H__