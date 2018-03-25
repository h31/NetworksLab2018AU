#pragma once

#include <vector>

namespace dns {

enum header_opcode {
    QUERY  = 0,
    IQUERY = 1,
    STATUS = 2
};

enum header_rcode {
    OK     = 0,
    FORMAT = 1,
    SERVER = 2,
    NAME   = 3,
    UNIMPL = 4,
    REFUSE = 5
};

#pragma pack(push,1)
struct header {
    unsigned short id;

    unsigned char rd:1;
    unsigned char tc:1;
    unsigned char aa:1;
    unsigned char opcode:4;
    unsigned char qr:1;

    unsigned char rcode:4;
    unsigned char z:3;
    unsigned char ra:1;
    
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};
#pragma pack(pop)

enum record_type {
    A       = 1,
    NS      = 2,
    CNAME   = 5,
    SOA     = 6,
    WKS     = 11,
    PTR     = 12,
    MX      = 15,
    SRV     = 33,
    AAAA    = 28,
    ANY     = 255
};

enum record_class {
    IN = 1
    // and many others
};

#pragma pack(push,1)
struct question_part { // constant-sized one
    unsigned short qtype;
    unsigned short qclass;
};
#pragma pack(pop)

struct question_record {
    std::string qname;
    question_part part;
};

struct question {
    std::vector<question_record> qs;
};

#pragma pack(push,1)
struct resource_record_part { // constant-sized one
    unsigned short type;
    unsigned short clss;
    unsigned int ttl;
    unsigned short rdlength;
};
#pragma pack(pop)

struct resource_record {
    std::string name;
    resource_record_part part;
    std::string rdata;
};

struct answer {
    std::vector<resource_record> rrs;
};

struct authority {
    std::vector<resource_record> rrs;
};

struct additional {
    std::vector<resource_record> rrs;
};

struct message {
    header h;
    question q;
    answer ans;
    authority auth;
    additional add;
    std::vector<char> raw_data; // for pointers
};

} // namespace dns