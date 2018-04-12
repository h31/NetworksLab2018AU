#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <vector>

#define QR_QUERY 0
#define QR_RESPONSE 1

#define OPCODE_QUERY 0 /* a standard query */
#define OPCODE_IQUERY 1 /* an inverse query */
#define OPCODE_STATUS 2 /* a server status request */

#define AA_NONAUTHORITY 0
#define AA_AUTHORITY 1

class Header {
public:
    uint16_t id;
    uint16_t qr:1;
    uint16_t opcode:4;
    uint16_t aa:1;
    uint16_t tc:1;
    uint16_t rd:1;
    uint16_t ra:1;
    uint16_t z:3;
    uint16_t rcode:4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

class dns_question {
public:
    std::string qname;
    uint16_t qtype;
    uint16_t qclass;
    int to_bytes(char * buffer, uint16_t idx);
};

class dns_response_packet {
public:
    std::string name;
    uint16_t type;
    uint16_t response_class;
    uint32_t ttl;
    uint16_t rdlength;
    std::string rdata;

    int to_bytes(char *buffer, uint16_t idx);
};

class dns_packet {
public:
    dns_packet(uint16_t id, uint16_t qr, const std::string& data);
    int dns_request_parse(char *data, u_int16_t size);
    void addRR(std::string domain_name, const std::string & ip);
    const char * get_data();

    uint16_t get_id();
    void set_id(uint16_t id);
    int to_bytes(char * buffer);
    ~dns_packet() {
        delete questions;
        delete RRs;
    }

    Header header;
    std::vector<dns_question> * questions;
    std::vector<dns_response_packet> * RRs;
private:
    int dns_header_parse(char *data);
    int dns_question_parse(char * data, uint16_t idx, uint16_t elem);
    int dns_RR_parse(char *data, uint16_t idx, uint16_t elem);
};

#endif //MESSAGE_H