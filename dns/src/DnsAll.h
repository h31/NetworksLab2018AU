#ifndef MESSENGER_ELEGRAMALL_H
#define MESSENGER_ELEGRAMALL_H

#include "DnsFwd.h"
#include "DnsError.h"
#include "DnsSocket.h"
#include "DnsServerSocket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#ifdef max
#undef max
#endif

//Main code reference:  https://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server

static bool inline is_correct_dns_resouce_record(int record) {
    return record == T_A
           || record == T_NS
           || record == T_CNAME
           || record == T_SOA
           || record == T_PTR
           || record == T_MX
            ;
}

struct DNS_HEADER {
    std::uint16_t id; // identification number
    
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
    
    std::uint16_t q_count; // number of question entries
    std::uint16_t ans_count; // number of answer entries
    std::uint16_t auth_count; // number of authority entries
    std::uint16_t add_count; // number of resource entries
};

#define DNS_HEADER_SIZE (sizeof(struct DNS_HEADER))

//Constant sized fields of query structure
struct QUESTION {
    std::uint16_t qtype;
    std::uint16_t qclass;
};

#define QUESTION_SIZE (sizeof(struct QUESTION))

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA {
    std::uint16_t type;
    std::uint16_t _class;
    std::uint32_t ttl;
    std::uint16_t data_len;
};
#pragma pack(pop)
#define R_DATA_SIZE (sizeof(R_DATA))

//Pointers to resource record contents
struct RES_RECORD {
    std::string name;
    R_DATA resource;
    std::string rdata;
};

struct QUERY {
    unsigned char *name;
    struct QUESTION *ques;
};

#define DNS_BUF_SIZE 65536
#define QUERY_TYPE T_A

static inline std::string read_name(unsigned char *reader, unsigned char *buffer, int &count) {
    count = 1;
    std::string result;
    bool jumped = false;
    
    assert(*reader != 0);
    //read the names in 3www6google3com format
    for (; *reader != 0; ++reader) {
        if (*reader >= 192) {
            auto tmp = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            assert(tmp >= 0);
            const auto offset = static_cast<const size_t>(tmp);
            reader = buffer + offset - 1;
            jumped = true; //we have jumped to another location so counting wont go up!
        } else {
            result += *reader;
//            jumped = false;
        }
        
        if (!jumped) {
            count++; //if we havent jumped to another location then we can count up
        }
    }
    
    if (jumped) {
        count++; //number of steps we actually moved forward in the packet
    }
    
    // convert 3www6google3com0 to www.google.com
    for (size_t i = 0; i < result.size(); ++i) {
        auto const p = static_cast<size_t>(result[i]);
        for (size_t j = 0; j < p; j++, i++) {
            result.at(i) = result.at(i + 1);
        }
        result[i] = '.';
    }
    assert(result.back() == '.');
    result.resize(result.size() - 1);
    return result;
}

static inline void hostname_in_dns(unsigned char *dns, std::string hostname) {
    hostname += ".";
    
    for (size_t i = 0, lock = 0; i < hostname.size(); i++) {
        if (hostname[i] == '.') {
            *dns++ = static_cast<unsigned char>(i - lock);
            for (; lock < i; lock++) {
                *dns++ = static_cast<unsigned char>(hostname[lock]);
            }
            lock++;
        }
    }
    *dns = '\0'; // *dns++ = '\0'.
}

#endif //MESSENGER_ELEGRAMALL_H
