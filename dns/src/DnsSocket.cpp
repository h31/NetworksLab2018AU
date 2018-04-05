#include <vector>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>
#include <cstring>

#include "DnsSocket.h"
#include "DnsAll.h"

//Main code reference:  https://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server

static bool is_correct_dns_resouce_record(int record) {
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

DnsSocket::DnsSocket() {
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries
}

void DnsSocket::set_dns_server(std::string &dns_server) {
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_server.c_str());
}

static std::string read_name(unsigned char *reader, unsigned char *buffer, int &count) {
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

void hostname_in_dns(unsigned char *dns, std::string hostname) {
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

static std::string address_from_answer(const RES_RECORD &answer) {
    auto resource_type = ntohs(answer.resource.type);
    assert(is_correct_dns_resouce_record(resource_type));
    if (resource_type == T_A) {
        auto *p = (long *) answer.rdata.c_str();
        sockaddr_in a{};
        a.sin_addr.s_addr = static_cast<in_addr_t>(*p); //working without ntohl
#if DEBUG_DNS
        printf("has IPv4 address: %s\n", inet_ntoa(a.sin_addr));
#endif
        return inet_ntoa(a.sin_addr);
    } else if (answer.resource.type == T_CNAME) { //Canonical name for an alias
#if DEBUG_DNS
        printf("has alias name: %s", answer.rdata.c_str());
#endif
        return answer.rdata;
    } else if (answer.resource.type == T_NS) {
#if DEBUG_DNS
        printf("has authoritative server: %s", answer.rdata.c_str());
#endif
        return answer.rdata;
    }
    assert(false);
}

#define BUF_SIZE 65536
#define QUERY_TYPE T_A
std::string DnsSocket::resolve(const std::string &hostname) {
    if (hostname.empty()) {
        throw DnsError("Empty hostname");
    }
    unsigned char buf[BUF_SIZE];
    auto *dns = reinterpret_cast<DNS_HEADER *>(&buf);
    bzero(buf, sizeof(buf));
    
    auto const dns_id = static_cast<uint16_t>(getpid() % std::numeric_limits<std::uint16_t>::max());
    dns->id = htons(dns_id);
//    dns->qr = 0; //This is a query
//    dns->opcode = 0; //This is a standard query
//    dns->aa = 0; //Not Authoritative
//    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
//    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
//    dns->z = 0;
//    dns->ad = 0;
//    dns->cd = 0;
//    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
//    dns->ans_count = 0;
//    dns->auth_count = 0;
//    dns->add_count = 0;
        
    //point to the query portion
    unsigned char *qname = &buf[DNS_HEADER_SIZE];
    
    hostname_in_dns(qname, hostname);
    auto const qname_length = strlen(reinterpret_cast<const char *>(qname));
    auto qinfo = (QUESTION *)&buf[DNS_HEADER_SIZE + (strlen((const char *) qname) + 1)]; //fill it
    
    qinfo->qtype = htons(QUERY_TYPE);
    qinfo->qclass = htons(1);

    auto const send_result = sendto(s, (char *) buf,
                                    DNS_HEADER_SIZE + (qname_length + 1) + QUESTION_SIZE, 0,
                                    (struct sockaddr *) &dest, sizeof(dest));
    if (send_result < 0) {
        throw DnsError("sendto failed");
    }
    
    // Receiving answer
    socklen_t dest_addr_len = sizeof dest;
    if (recvfrom(s, buf, BUF_SIZE, 0, reinterpret_cast<sockaddr *>(&dest), &dest_addr_len) < 0) {
        throw DnsError("recvfrom failed");
    }
    
    dns = reinterpret_cast<DNS_HEADER *>(buf);
    
    //move ahead of the dns header and the query field
    // update qname_length?
    unsigned char *reader = &buf[DNS_HEADER_SIZE + (qname_length + 1) + QUESTION_SIZE];

#if DEBUG_DNS
    printf("\nThe response contains : ");
    printf("\n %d Questions.", ntohs(dns->q_count));
    printf("\n %d Answers.", ntohs(dns->ans_count));
    printf("\n %d Authoritative Servers.", ntohs(dns->auth_count));
    printf("\n %d Additional records.\n\n", ntohs(dns->add_count));
#endif
    
    int stop = 0;
    
    auto read_answer = [&]() {
        RES_RECORD answer{};
        answer.name = read_name(reader, buf, stop);
        reader += stop;
#if DEBUG_DNS
        printf("Name: %s\t", answer.name.c_str());
#endif
        memcpy(&answer.resource, reader, R_DATA_SIZE);
        reader += R_DATA_SIZE;
    
        auto const query_type = ntohs(answer.resource.type);

        if (query_type == T_A) { // ipv4 address.
            auto const resource_data_len = ntohs(answer.resource.data_len);
            answer.rdata = std::string(resource_data_len, ' ');
    
            for (size_t j = 0; j < resource_data_len; ++j) {
                answer.rdata[j] = reader[j];
            }
            reader += resource_data_len;
        } else {
            answer.rdata = read_name(reader, buf, stop);
            reader += stop;
        }

#if DEBUG_DNS
        auto const address_str = address_from_answer(answer);
//        printf("Address: %s\n", address_str.c_str());
#endif
        return answer;
    };
    
    auto read_auth = [&]() {
        RES_RECORD answer{};
        answer.name = read_name(reader, buf, stop);
        reader += stop;
        printf("Name: %s\n", answer.name.c_str());
    
        memcpy(&answer.resource, reader, R_DATA_SIZE);
        reader += R_DATA_SIZE;
        
        answer.rdata = read_name(reader, buf, stop);
        reader += stop;
        return answer;
    };

    assert(ntohs(dns->add_count) == 0);
    std::vector<RES_RECORD> answers;
    
    auto const ans_count = ntohs(dns->ans_count);
    if (ans_count) {
#if DEBUG_DNS
        printf("Answers: \n");
#endif
        std::generate_n(std::back_inserter(answers), ans_count, read_answer);
    }
    
    auto const auth_count = ntohs(dns->auth_count);
    if (auth_count) {
#if DEBUG_DNS
        printf("Authoritative records: \n");
#endif
        std::generate_n(std::back_inserter(answers), auth_count, read_auth);
    }
    
    if (answers.empty()) {
        throw DnsError("No answers received");
    }
    
    return address_from_answer(answers[0]);
    
//    //read authorities
//    for (int i = 0; i < ntohs(dns->auth_count); i++) {
//        auth[i].name = ReadName(reader, buf, &stop);
//        reader += stop;
//
//        auth[i].resource = (struct R_DATA *) (reader);
//        reader += sizeof(struct R_DATA);
//
//        auth[i].rdata = ReadName(reader, buf, &stop);
//        reader += stop;
//    }
//
//    //read additional
//    for (int i = 0; i < ntohs(dns->add_count); i++) {
//        addit[i].name = ReadName(reader, buf, &stop);
//        reader += stop;
//
//        addit[i].resource = (struct R_DATA *) (reader);
//        reader += sizeof(struct R_DATA);
//
//        if (ntohs(addit[i].resource->type) == 1) {
//            addit[i].rdata = (unsigned char *) malloc(ntohs(addit[i].resource->data_len));
//            for (j = 0; j < ntohs(addit[i].resource->data_len); j++)
//                addit[i].rdata[j] = reader[j];
//
//            addit[i].rdata[ntohs(addit[i].resource->data_len)] = '\0';
//            reader += ntohs(addit[i].resource->data_len);
//        } else {
//            addit[i].rdata = ReadName(reader, buf, &stop);
//            reader += stop;
//        }
//    }
//
//
//    //print authorities
//    printf("\nAuthoritive Records : %d \n", ntohs(dns->auth_count));
//    for (i = 0; i < ntohs(dns->auth_count); i++) {
//
//        printf("Name : %s ", auth[i].name);
//        if (ntohs(auth[i].resource->type) == 2) {
//            printf("has nameserver : %s", auth[i].rdata);
//        }
//        printf("\n");
//    }
//
//    //print additional resource records
//    printf("\nAdditional Records : %d \n", ntohs(dns->add_count));
//    for (i = 0; i < ntohs(dns->add_count); i++) {
//        printf("Name : %s ", addit[i].name);
//        if (ntohs(addit[i].resource->type) == 1) {
//            long *p;
//            p = (long *) addit[i].rdata;
//            a.sin_addr.s_addr = (*p);
//            printf("has IPv4 address : %s", inet_ntoa(a.sin_addr));
//        }
//        printf("\n");
//    }
}
