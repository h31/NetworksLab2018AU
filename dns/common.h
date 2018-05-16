#ifndef __COMMON__
#define __COMMON__

#include <cstdio>
#include <cstdlib>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>

#define DNS_PORT 53
#define MAX_MSG_LEN 256
#define TTL 300

struct DnsHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answerRRs;
    uint16_t autorityRRs;
    uint16_t additionalRRs;
};


struct DnsQuery {
    char* domain;
    uint16_t type;
    uint16_t clazz;
};

struct DnsRequest {
    //Header
    DnsHeader* header;
    //Query
    DnsQuery* query;
};

struct DnsResponse {
    //Header
    DnsHeader* header;
    //Query
    DnsQuery* query;
    //Response
    char* name;
    uint16_t type;
    uint16_t clazz;
    uint32_t ttl;
    uint16_t dataLen;
    uint8_t ip[4];
};

int readDnsRequest(DnsRequest &dnsRequest, uint8_t *buffer);

int readDnsResponse(DnsResponse &dnsReqponse, uint8_t *buffer);

int writeDnsRequest(DnsRequest &dnsRequest, uint8_t *buffer);

int writeDnsResponse(DnsResponse &dnsReqponse, uint8_t *buffer);

#endif
