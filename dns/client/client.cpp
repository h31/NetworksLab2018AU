//
// Created by kate on 26.03.18.
//

// источник: https://www.binarytides.com/dns-query-code-in-c-with-winsock/

#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>    //getpid

#include "../common/common.h"

const static int PORT = 53;

/*
 * this will convert www.google.com
 * to 3www6google3com;
 */
static void to_dns_name(unsigned char *dns, const char *const host) {
    size_t lock = 0;
    for (size_t i = 0; i < strlen(host) + 1; i++) {
        if (host[i] == '.' || host[i] == 0) {
            *dns++ = i - lock;
            memcpy(dns, host + lock, i - lock);
            dns += i - lock;
            lock += i - lock + 1;
        }
    }
    *dns = 0;
}

static unsigned char *read_name(unsigned char *reader, unsigned char *buffer, int *count) {
    *count = 1;
    unsigned char *name = (unsigned char *) malloc(256);
    int p = 0;
    char jumped = 0;
    for (; *reader; p++) {
        if (*reader >= 192) {
            const int offset = (*reader) * 256 + *(reader + 1) - 49152;
            reader = buffer + offset;
            --p;
            jumped = 1;
        } else {
            name[p] = *reader;
            reader += 1;
        }

        *count += !jumped;
    }
    name[p] = 0;
    *count += jumped;

    size_t i = 0;
    for (; i < strlen((char *) name); i++) {
        p = name[i];
        memcpy(name + i, name + i + 1, p);
        i += p;
        name[i] = '.';
    }
    name[i > 0 ? i - 1 : 0] = 0;
    return name;
}


void print_dns(struct DNS_HEADER *dns) {
    std::cout << "Recv: questions=" << ntohs(dns->q_count) << " answers="
              << ntohs(dns->ans_count) << " auto_serv="
              << ntohs(dns->auth_count) << " records="
              << ntohs(dns->add_count) << std::endl;

    std::cout << "answer records: " << ntohs(dns->ans_count) << std::endl;
}

int main(int argc, char **argv) {


    if (argc < 3) {
        std::cout << "client dns_addr addr" << std::endl;
        exit(1);
    }

    const char *const dns_server = argv[1];
    const char *const address = argv[2];


    int server_id = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_id < 0) {
        std::cout << "ERROR: open socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in addr = {};
    memset((char *) &addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (inet_aton(dns_server, &addr.sin_addr) <= 0) {
        std::cout << "ERROR: inet_aton " << argv[1] << std::endl;
        exit(1);
    }

    unsigned char buffer[MAX_UDP_SIZE] = {};
    struct DNS_HEADER *dns = (struct DNS_HEADER *) buffer;
    dns->id = (unsigned short) htons(getpid());
    dns->q_count = htons(1); //we have only 1 question
    auto *qname = (unsigned char *) (buffer + sizeof(struct DNS_HEADER));
    to_dns_name(qname, address);
    auto *qinfo = (struct QUESTION *) (buffer +
                                       sizeof(struct DNS_HEADER) +
                                       strlen((char *) qname) + 1);
    qinfo->qtype = htons(T_A);
    qinfo->qclass = htons(1); // internet

    std::cout << "send packet " << server_id << std::endl;
    if (sendto(server_id, (char *) buffer,
               sizeof(struct DNS_HEADER) + strlen((const char *) qname) +
               sizeof(struct QUESTION) + 1, 0,
               (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cout << " ERROR: send packet" << std::endl;
        exit(1);
    }

    bzero(buffer, MAX_UDP_SIZE);
    std::cout << "recv answer" << std::endl;
    size_t len_addr = sizeof(addr);
    if (recvfrom(server_id, (char *) buffer, MAX_UDP_SIZE, 0,
                 (struct sockaddr *) &addr, (socklen_t * ) & len_addr)
        == -1) {
        std::cout << "ERROR: recvfrom " << std::endl;
        exit(1);
    }
    print_dns(dns);

    unsigned char *reader = buffer + sizeof(struct DNS_HEADER) +
                            strlen((const char *) qname) +
                            sizeof(struct QUESTION) + 1;
    for (int i = 0; i < ntohs(dns->ans_count); i++) {
        int offset = 0;
        unsigned char *name = read_name(reader, buffer, &offset);
        std::cout << " name: " << name << std::endl;
        reader += offset;
        struct R_DATA *data = (struct R_DATA *) reader;
        reader += sizeof(struct R_DATA);
        if (ntohs(data->type) == T_A) {
            unsigned char *rdata = new unsigned char[ntohs(data->data_len)];
            memcpy(rdata, reader, ntohs(data->data_len));
            rdata[ntohs(data->data_len)] = 0;
            reader += ntohs(data->data_len);

            struct sockaddr_in a = { };
            memset((char *) &a, 0, sizeof(a));
            a.sin_addr.s_addr = *(int *) rdata;


            std::cout << " IPv4 address: " << inet_ntoa(a.sin_addr) <<
                      std::endl;
            delete[]rdata;
        } else {
            int offset = 0;
            unsigned char *rdata = read_name(reader, buffer, &offset);
            reader = reader + offset;
            if (ntohs(data->type) == 5) {
                std::cout << "has alias name: " << rdata << std::endl;
            }
            delete[] rdata;
        }
        delete[]name;
        std::cout << "\n" << std::endl;
    }
    return 0;
}