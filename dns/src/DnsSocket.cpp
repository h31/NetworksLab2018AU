#include <vector>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>
#include <cstring>

#include "DnsSocket.h"
#include "DnsAll.h"


DnsSocket::DnsSocket() {
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries
}

DnsSocket::DnsSocket(const std::string &dns_server, int port)
        : DnsSocket()
{
    set_dns_server(dns_server, port);
}

void DnsSocket::set_dns_server(const std::string &dns_server, int port) {
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = inet_addr(dns_server.c_str());
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

std::string DnsSocket::resolve(const std::string &hostname) {
    if (hostname.empty()) {
        throw DnsError("Empty hostname");
    }
    unsigned char buf[DNS_BUF_SIZE];
    auto *const dns = reinterpret_cast<DNS_HEADER *>(&buf);
    bzero(buf, sizeof(buf));
    
    auto const dns_id = static_cast<uint16_t>(getpid() % std::numeric_limits<std::uint16_t>::max());
    dns->id = htons(dns_id);
//    dns->qr = 0; //This is a query
//    dns->opcode = 0; //This is a standard query
//    dns->aa = 0; //Not Authoritative
//    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
//    dns->ra = 0; //Recursion not available
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
    auto qinfo = (QUESTION *)&buf[DNS_HEADER_SIZE + qname_length + 1]; //fill it
    
    qinfo->qtype = htons(QUERY_TYPE);
    qinfo->qclass = htons(1);
    
    auto const send_result = sendto(s, (char *) buf,
                                    DNS_HEADER_SIZE + (qname_length + 1) + QUESTION_SIZE, 0,
                                    reinterpret_cast<const sockaddr *>(&dest), sizeof dest);
    if (send_result < 0) {
        throw DnsError("sendto failed.");
    }
    
    // Receiving answer
    socklen_t dest_addr_len = sizeof dest;
    if (recvfrom(s, buf, DNS_BUF_SIZE, 0, reinterpret_cast<sockaddr *>(&dest), &dest_addr_len) < 0) {
        throw DnsError("recvfrom failed");
    }
    
    assert(dest_addr_len == sizeof dest);
    
//    dns = reinterpret_cast<DNS_HEADER *>(buf);
    
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
