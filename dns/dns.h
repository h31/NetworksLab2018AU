#ifndef DNS_DNS_H
#define DNS_DNS_H
// http://www.fauser.edu/~fuligni/files/classi5/sistemi-reti/project1-primer%20(DNS%20message%20structure).pdf
// http://www.networksorcery.com/enp/Protocol/dns.htm

#define REQUEST_FLAG 0x0100
#define RESPONSE_FLAG 0x8180
#define MAX_DNS_MESSAGE_LENGTH 4096

enum dns_type {
    A = 1
};

enum dns_class {
    IN = 1
};
struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

struct dns_question {
    char* qname;
    enum dns_type type;
    enum dns_class class;
};

struct dns_answer {
    uint16_t name;
    enum dns_type type;
    enum dns_class class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t* rddata;
};

int write16(char* buffer, uint16_t x);
int write32(char* buffer, uint32_t x);
uint16_t read16(const char* buffer);
#endif //DNS_DNS_H
