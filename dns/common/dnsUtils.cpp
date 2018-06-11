#include "dnsUtils.h"

#include<string.h>    //strlen
#include<arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include<unistd.h>    //getpid
#include <stdexcept>

// потырил много отсюда: https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server

struct DNS_HEADER {
    unsigned short id; // identification number

    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag

    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available

    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 1)
struct R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

/*
 * This will convert www.google.com to 3www6google3com0
 * */
void ChangeToDnsNameFormat(unsigned char *dns, const char *host) {
    int lock = 0, i;
    strcat((char *) host, ".");

    for (i = 0; i < strlen((char *) host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++ = '\0';
}

u_char *ReadFromDnsNameFormat(unsigned char *reader, unsigned char *buffer, int *count) {
    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *) malloc(256);

    name[0] = '\0';

    //read the names in 3www6google3com format
    while (*reader != 0) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        } else {
            name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0) {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }

    name[p] = '\0'; //string complete
    if (jumped == 1) {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }

    //now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int) strlen((const char *) name); i++) {
        p = name[i];
        for (j = 0; j < (int) p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; //remove the last dot
    return name;
}

size_t writeDNSARequest(std::vector<char> &buf, const std::string &hostname) {
    struct DNS_HEADER *dns = nullptr;

    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *) buf.data();

    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    unsigned char *qname = (unsigned char *) &buf[sizeof(struct DNS_HEADER)];
    ChangeToDnsNameFormat(qname, hostname.c_str());

    struct QUESTION *qinfo = nullptr;
    qinfo = (struct QUESTION *) &buf[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1)]; //fill it

    qinfo->qtype = htons(T_A); //type of the query , A , MX , CNAME , NS etc
    qinfo->qclass = htons(1); //its internet (lol)

    size_t totalSize = sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION);
    return totalSize;
}

std::string parseIPFromDNSResponse(std::vector<char> &buf) {
    DNS_HEADER *dns = (struct DNS_HEADER *) buf.data();

    if (ntohs(dns->ans_count) < 1) {
        return "unknown hostname";
    }

    unsigned char *reader;
    reader = (unsigned char *) &buf[sizeof(DNS_HEADER)];
    reader += strlen((const char *) reader) + 1;
    reader += sizeof(QUESTION);

    int stop = 0;
    unsigned char *name = ReadFromDnsNameFormat(reader, (unsigned char *) buf.data(), &stop);
    reader += stop;

    R_DATA *resource = (R_DATA *) reader;
    reader = reader + sizeof(R_DATA);

    if (ntohs(resource->type) != 1) {
        return "unexpected answer";
    }

    unsigned char *rdata = (unsigned char *) malloc(ntohs(resource->data_len));

    for (unsigned short j = 0; j < ntohs(resource->data_len); j++) {
        rdata[j] = reader[j];
    }
    rdata[ntohs(resource->data_len)] = '\0';

    sockaddr_in a{};
    a.sin_addr.s_addr = *((in_addr_t *) rdata); //working without ntohl
    std::string result(inet_ntoa(a.sin_addr));
    free(rdata);

    return result;
}

size_t writeDNSAResponse(std::vector<char> &buf, const std::string &hostname, const std::string &ipAddress) {
    int shift = 0;
    auto *dns = (DNS_HEADER *) &buf[shift];
    shift += sizeof(struct DNS_HEADER);

    dns->qr = 1;
    dns->ans_count = htons(1);

    auto *qname = (unsigned char *) &buf[shift];
    ChangeToDnsNameFormat(qname, hostname.c_str());
    shift += (strlen((const char *) qname) + 1);

    auto *qinfo = (QUESTION *) &buf[shift];
    shift += sizeof(struct QUESTION);
    qinfo->qtype = htons(T_A);
    qinfo->qclass = htons(1);

    if (ipAddress != "0.0.0.0") {
        qname = (unsigned char *) &buf[shift];
        ChangeToDnsNameFormat(qname, hostname.c_str());
        shift += (strlen((const char *) qname) + 1);

        in_addr_t addr = inet_addr(ipAddress.c_str());

        auto *resource = (R_DATA *) &buf[shift];
        shift += sizeof(R_DATA);
        resource->type = htons(T_A);
        resource->data_len = htons(sizeof(addr));
        resource->_class = htons(1);
        resource->ttl = htonl(200);

        union {
            in_addr_t addr;
            char bytes[4];
        } u = {addr};

        char *rdata = &buf[shift];
        shift += sizeof(in_addr_t);

        rdata[0] = u.bytes[0];
        rdata[1] = u.bytes[1];
        rdata[2] = u.bytes[2];
        rdata[3] = u.bytes[3];
    }

    auto totalSize = static_cast<size_t>(shift);
    return totalSize;
}

std::string parseHostNameFromDNSRequest(std::vector<char> &buf) {
    auto *reader = (unsigned char *) &buf[sizeof(DNS_HEADER)];
    int stop = 0;
    unsigned char *name = ReadFromDnsNameFormat(reader, (unsigned char *) buf.data(), &stop);
    std::string result((const char *) name);
    return result;
}
