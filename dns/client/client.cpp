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



char buffer[MAX_UDP_SIZE];
const static int PORT = 1234;

/*
 * this will convert www.google.com
 * to 3www6google3com;
 */
void change_to_dns_name_format(char *dns, const char *const host) {
    int src = 0, des = 0, len = 0; // Some positions.
    dns[len] = 0;
    while (host[src] != '\0') {
        if (host[src] == '.') {
            len += dns[len] + 1;
            dns[len] = 0;
            des = len;
            ++src; // Ignore '.'
        } else {
            dns[++des] = host[src++];
            ++dns[len];
        }
    }
    dns[++des] = '\0';
}


void print_dns(struct DNS_HEADER *dns) {
    std::cout << "Recv: questions=" << ntohs(dns->q_count) << " answers="
              << ntohs(dns->ans_count) << " auto_serv="
              << ntohs(dns->auth_count) << " records="
              << ntohs(dns->add_count) << std::endl;

    std::cout << "answer records: " << ntohs(dns->ans_count) << std::endl;
}


char *read_name(char *reader, char *buffer, int *count) {

    int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    auto *name = new char[256];

    name[0] = '\0';

    //read the names in 3www6google3com format
    while (*reader != 0) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) -
                     49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        } else {
            name[p++] = *reader;
        }
        reader = reader + 1;
        if (jumped == 0) {
            *count = *count + 1;
            //if we havent jumped to another location then we can count up
        }
    }
    name[p] = '\0'; //string complete
    if (jumped == 1) {
        *count = *count + 1;
        //number of steps we actually moved forward in the packet
    }

    //now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int) strlen(name); i++) {
        p = name[i];
        for (j = 0; j < p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; //remove the last dot
    return name;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "client dns_addr addr" << std::endl;
        exit(1);
    }

    int server_id = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_id < 0) {
        std::cout << "ERROR: open socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in addr = {};
    memset((char *) &addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_aton(argv[1], &addr.sin_addr) <= 0) {
        std::cout << "ERROR: inet_aton " << argv[1] << std::endl;
        exit(1);
    }

    auto *dns = (struct DNS_HEADER *) buffer;
    dns->id = (short) htons(static_cast<uint16_t>(getpid()));
    dns->q_count = htons(1); //we have only 1 question
    auto *qname = buffer + DNS_HEADER_SIZE;
    change_to_dns_name_format(qname, argv[2]);
    auto *qinfo = (struct QUESTION *) (buffer +
                                       DNS_HEADER_SIZE +
                                       strlen(qname) + 1);
    qinfo->qtype = htons(1);
    qinfo->qclass = htons(1); // internet

    std::cout << "send packet " << server_id << std::endl;
    if (sendto(server_id, buffer, DNS_HEADER_SIZE + strlen(qname) + 1 +
                                  QUESTION_SIZE, 0,
               (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cout << " ERROR: send packet" << std::endl;
        exit(1);
    }

    bzero(buffer, MAX_UDP_SIZE);
    std::cout << "recv answer" << std::endl;
    size_t len_addr = sizeof(addr);
    if (recvfrom(server_id, buffer, MAX_UDP_SIZE, 0,
                 (struct sockaddr *) &addr, (socklen_t *) &len_addr)
        == -1) {
        std::cout << "ERROR: recvfrom " << std::endl;
        exit(1);
    }
    print_dns(dns);

    char *reader = buffer + DNS_HEADER_SIZE +
                   strlen(qname) + 1 + QUESTION_SIZE;
    for (int i = 0; i < ntohs(dns->ans_count); i++) {
        int offset = 0;
        char *name = read_name(reader, buffer, &offset);
        std::cout << " name: " << name << std::endl;
        reader += offset;
        auto *data = (struct R_DATA *) reader;
        reader += R_DATA_SIZE;
        if (ntohs(data->type) == 1) {
            auto *rdata = new char[ntohs(data->data_len)];
            memcpy(rdata, reader, ntohs(data->data_len));
            rdata[ntohs(data->data_len)] = 0;
            reader += ntohs(data->data_len);

            struct sockaddr_in a = {};
            a.sin_addr.s_addr = static_cast<in_addr_t>(*(int *) rdata);

            std::cout << " IPv4 address: " << inet_ntoa(a.sin_addr) <<
                      std::endl;
            delete[]rdata;
        } else {
            offset = 0;
            char *rdata = read_name(reader, buffer, &offset);
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