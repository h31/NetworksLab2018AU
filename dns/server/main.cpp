#include <thread>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

#include "message.h"

const size_t MAX_SIZE = 256;
const size_t BUFFER_SIZE = 65536;

int getSO_ERROR(int fd) {
    int err = 1;
    socklen_t len = sizeof err;
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *) &err, &len)) {
        std::cerr << "getSO_ERROR";
        exit(1);
    }
    if (err)
        errno = err;
    return err;
}

void closeSocket(int fd) {
    if (fd >= 0) {
        getSO_ERROR(fd);
        if (shutdown(fd, SHUT_RDWR) < 0)
            if (errno != ENOTCONN && errno != EINVAL)
                std::cerr << "shutdown";
        if (close(fd) < 0)
            std::cerr << "close";
    }
}

void decode(unsigned char *buffer) {
    size_t i = 0;
    for (; i < strnlen(reinterpret_cast<const char *>(buffer), 256); i++) {
        size_t offset_size = buffer[i];
        memcpy(buffer + i, buffer + i + 1, offset_size);
        i += offset_size;
        buffer[i] = '.';
    }
    if (i > 0) {
        buffer[i - 1] = 0;
    } else {
        buffer[0] = 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR number of args";
        exit(1);
    }
    auto portno = static_cast<uint16_t>(atoi(argv[1]));
    unsigned char buffer[65536]{};
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        std::cerr << "ERROR opening socket";
        exit(1);
    }

    sockaddr_in si_me{};
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(portno);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
        std::cerr << "ERROR bind";
        closeSocket(sockfd);
        exit(1);
    }
    while (true) {
        sockaddr_in si_other{};
        int slen = sizeof(si_other);
        if ((recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &si_other,
                      reinterpret_cast<socklen_t *>(&slen))) == -1) {
            std::cerr << "ERROR recvfrom";
        }
        header *dns_header = reinterpret_cast<header *>(buffer);
        dns_header->QR = 1;
        dns_header->ANCOUNT = htons(1);
        auto *qname = reinterpret_cast<char *>(&buffer[sizeof(header)]);
        auto *copy_name = new char[MAX_SIZE];
        strncpy(copy_name, qname, MAX_SIZE);
        decode(reinterpret_cast<unsigned char *>(qname));
        std::cout << qname << std::endl;
        auto *reader = reinterpret_cast<char *>(&buffer[sizeof(header) + strlen(qname) + 1 + sizeof(dns_question)]);
        strcpy(reader, copy_name);
        reader += strlen(copy_name) + 1;
        dns_rdata *rdata = reinterpret_cast<dns_rdata *>(reader);
        rdata->TYPE = htons(1);
        rdata->TTL = htons(200);
        rdata->CLASS = htons(1);
        rdata->RDLENGTH = htons(4);
        reader += sizeof(dns_rdata);
        hostent *hosten = gethostbyname(qname);
        if (hosten == nullptr) {
            rdata->RDLENGTH = htons(0);
        } else {
            *(int *) reader = ((in_addr *) hosten->h_addr_list[0])->s_addr;
        }
        if (sendto(sockfd,
                   buffer,
                   sizeof(header) + strlen(qname) + 1 + sizeof(dns_question) + strlen(copy_name) + 1 +
                   sizeof(dns_rdata) + ntohs(rdata->RDLENGTH), 0,
                   reinterpret_cast<const sockaddr *>(&si_other),
                   static_cast<socklen_t>(slen)) == -1) {
            std::cerr << "ERROR sendto";
        }
        delete[]copy_name;
        std::cout << "Done." << std::endl;
    }
}
