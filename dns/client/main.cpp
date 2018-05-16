#include <thread>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "message.h"
#include "utils.h"

const size_t BUFFER_SIZE = 65536;

int getSO_ERROR(int fd) {
    int err = 1;
    socklen_t len = sizeof err;
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *) &err, &len)) {
        std::cerr << "getSO_ERROR";
        exit(1);
    }
    if (err) {
        errno = err;
    }
    return err;
}

void closeSocket(int fd) {
    if (fd >= 0) {
        getSO_ERROR(fd);
        if (shutdown(fd, SHUT_RDWR) < 0) {
            if (errno != ENOTCONN && errno != EINVAL) {
                std::cerr << "shutdown";
            }
        }
        if (close(fd) < 0) {
            std::cerr << "close";
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "ERROR number of args";
        exit(1);
    }
    unsigned char buffer[BUFFER_SIZE]{};
    uint16_t portno = static_cast<uint16_t>(atoi(argv[2]));
    header *dns_header = reinterpret_cast<header *>(&buffer);
    dns_header->ID = (uint16_t) htons(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    dns_header->RD = 1;
    dns_header->QDCOUNT = htons(1);
    unsigned char *qname = &buffer[sizeof(header)];
    encode(qname, argv[3]);
    dns_question *question = reinterpret_cast<dns_question *>(&buffer[sizeof(header) +
                                                                      strlen(reinterpret_cast<const char *>(qname)) +
                                                                      1]);
    question->QCLASS = htons(1);
    question->QTYPE = htons(1);
    sockaddr_in dest{
            .sin_family = AF_INET,
            .sin_port = htons(portno),
            .sin_addr.s_addr = inet_addr(argv[1])
    };
    int dest_size = sizeof(dest);
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    std::cout << "Sending packet...";
    if (sendto(sockfd,
               buffer,
               sizeof(header) + strlen(reinterpret_cast<const char *>(qname)) + 1 + sizeof(dns_question),
               0,
               reinterpret_cast<sockaddr *>(&dest),
               static_cast<socklen_t>(dest_size)) == -1) {
        std::cerr << "ERROR sendto";
        close(sockfd);
        exit(1);
    }
    std::cout << "Sent";
    std::cout << "\nReceiving answer...";
    std::cout.flush();
    bzero(buffer, BUFFER_SIZE);
    if (recvfrom(sockfd,
                 buffer,
                 BUFFER_SIZE,
                 0,
                 reinterpret_cast<sockaddr *>(&dest),
                 reinterpret_cast<socklen_t *>(&dest_size)) == -1) {
        std::cerr << "ERROR recvfrom";
        close(sockfd);
        exit(1);
    }
    std::cout << "Received" << std::endl;
    dns_header = reinterpret_cast<header *>(buffer);
    unsigned char *reader = buffer + sizeof(header) + strlen((const char *) qname) + 1 + sizeof(dns_question);
    for (size_t i = 0; i < ntohs(dns_header->ANCOUNT); ++i) {
        auto *name = new unsigned char[MAX_SIZE];
        reader += get_name(reader, buffer, name);
        decode(name);
        auto *data = (dns_rdata *) reader;
        reader += sizeof(dns_rdata);
        if (ntohs(data->TYPE) == 1) {
            std::cout << name << "\t";
            auto *rdata = new unsigned char[ntohs(data->RDLENGTH)];
            memcpy(rdata, reader, ntohs(data->RDLENGTH));
            rdata[ntohs(data->RDLENGTH)] = 0;
            reader += ntohs(data->RDLENGTH);
            sockaddr_in ip = {
                    .sin_addr.s_addr = *(unsigned int *) rdata
            };
            std::cout << "IP: " << inet_ntoa(ip.sin_addr) << std::endl;
            delete[]rdata;
        } else {
            reader = reader + get_name(reader, buffer, name);
        }
        delete[]name;
    }
    closeSocket(sockfd);
    return 0;
}