#include <thread>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "message.h"

int getSO_ERROR(int fd) {
    int err = 1;
    socklen_t len = sizeof err;
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len)) {
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
        if (close(fd) < 0) // finally call close()
            std::cerr << "close";
    }
}

int main(int argc, char* argv[]){
    if (argc != 4) {
        std::cerr << "ERROR number of args";
        exit(1);
    }
    size_t buffer_size = 65536;
    unsigned char buffer[65536]{};
    char* qname;
    uint16_t portno;
    header* dns_header;
    dns_question* question;
    portno = static_cast<uint16_t>(atoi(argv[2]));
    dns_header = reinterpret_cast<header *>(&buffer);
    dns_header->ID = (uint16_t)htons(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    dns_header->RD = 1;
    dns_header->QDCOUNT = htons(1);
    qname = reinterpret_cast<char *>(&buffer[sizeof(header)]);
    strcpy(qname, argv[3]);
    question = reinterpret_cast<dns_question *>(&buffer[sizeof(header) + strlen(qname) + 1]);
    question->QCLASS = htons(1);
    question->QTYPE = htons(1);
    int sockfd;
    sockaddr_in dest{
            .sin_family = AF_INET,
            .sin_port = htons(portno),
            .sin_addr.s_addr = inet_addr(argv[1])
    };
    int dest_size = sizeof(dest);
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    std::cout << "Sending packet...";
    if (sendto(sockfd,
               buffer,
               sizeof(header) + strlen(qname) + 1 + sizeof(dns_question),
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
    if (recvfrom(sockfd,
                 buffer,
                 buffer_size,
                 0,
                 reinterpret_cast<sockaddr *>(&dest),
                 reinterpret_cast<socklen_t *>(&dest_size)) == -1) {
        std::cerr << "ERROR recvfrom";
        close(sockfd);
        exit(1);
    }
    std::cout << "Received" << std::endl;
    char* reader;
    dns_header = reinterpret_cast<header *>(buffer);
    reader = reinterpret_cast<char *>(&buffer[sizeof(header) + strlen(qname) + 1 + sizeof(dns_question)]);
    rr ans{};
    for (size_t i = 0; i < ntohs(dns_header->ANCOUNT); ++i) {
        ans.NAME = reader;
        reader += strlen(ans.NAME) + 1;
        ans.res = reinterpret_cast<dns_rdata *>(reader);
        reader += sizeof(dns_header);
        ans.RDATA = reader;
        reader += strlen(ans.RDATA) + 1;
        std::cout << ans.NAME << "\t:\t" << inet_ntoa(*(in_addr*)ans.RDATA) << std::endl;
    }
    closeSocket(sockfd);
    return 0;
}