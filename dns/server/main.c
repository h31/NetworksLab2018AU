#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <message.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "message.h"

#define DEFAULT_TTL 100
#define BUF_SIZE 65536
#define RESOURCE_NAME_SIZE 256

void get_options(int argc, char** argv, uint16_t* port);

int main(int argc, char **argv) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;     // Fill IP auto

    uint16_t portno;
    get_options(argc, argv, &portno);
    char server_port[6];
    snprintf(server_port, 6, "%u", portno);

    if (getaddrinfo(NULL, server_port, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        return 1;
    }
    int sockfd;
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("ERROR: Could not create socket");
        return 1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("ERROR on binding");
        return 1;
    }

	uint8_t buffer[BUF_SIZE];
    while (1) {
        struct sockaddr client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        ssize_t count = recvfrom(sockfd, buffer, BUF_SIZE, 0,
                                 &client_addr, &client_addr_len);
        if (count < 0) {
            fprintf(stderr, "failed to receive header from client\n");
            continue;
        }
        header_t* header = (header_t*)buffer;
		if (ntohs(header->qdcount) > 0) {
			// answer
			struct addrinfo *answer_addr;
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;

			uint32_t offset = sizeof(header_t);
			uint8_t* name = buffer + offset;
			char addr[RESOURCE_NAME_SIZE];
			uint8_t label = (uint8_t)(name[0] & 0x3f);
			uint8_t start = 0;
			while (label != 0) {
				for (uint8_t index = 0; index < label; ++index) {
					addr[start + index] = name[start + index + 1];
				}
				start += label;
				if ((label = (uint8_t)(name[start + 1] & 0x3f)) != 0) {
					addr[start] = '.';
				} else {
					addr[start] = '\0';
				}
				++start;
			}

			uint8_t name_length = start;
			int addr_res = getaddrinfo(addr, NULL, &hints, &answer_addr);
			uint8_t rcode;
			switch (addr_res) {
				case 0:
					rcode = 0;
					break;
				case EAI_AGAIN:
					rcode = 2;
					break;
				case EAI_BADFLAGS:
					rcode = 1;
					break;
				case EAI_FAIL:
					rcode = 2;
					break;
				case EAI_FAMILY:
					rcode = 4;
					break;
				case EAI_MEMORY:
					rcode = 2;
					break;
				case EAI_NONAME:
					rcode = 1;
					break;
				case EAI_SERVICE:
					rcode = 3;
					break;
				case EAI_SOCKTYPE:
					rcode = 1;
					break;
				case EAI_SYSTEM:
					rcode = 2;
					break;
				default:
					rcode = 2;
					break;
			}
			header->qr = 1;
			header->aa = 0;
			header->ra = 1;
			header->rcode = rcode;
			header->arcount = 0;
			header->nscount = 0;
			header->qdcount = htons(1);
			if (header->rcode != 0) {
				header->ancount = 0;
			} else {
				header->ancount = htons(1);
			}

			offset += strlen((const char*)name) + 1 + sizeof(question_t);
			if (header->ancount != 0) {
				char* resource_name_ptr = (char *) (buffer + offset);
				memcpy(resource_name_ptr, name, name_length + 1);
				offset += name_length + 1;
				answer_t* answer = (answer_t*) (buffer + offset);
				answer->type = htons(TYPE_A);
				answer->class = htons(CLASS_IN);
				answer->ttl = htonl(DEFAULT_TTL);
				answer->rdlength = htons(4);
				struct sockaddr_in *ipv4 = (struct sockaddr_in *)answer_addr->ai_addr;
				char* ip = inet_ntoa(ipv4->sin_addr);
//			printf("%s\t%s\n", addr, ip);
				uint8_t octet = 0;
				offset += sizeof(answer_t);
				uint8_t* rdata = (uint8_t*) (buffer + offset);
				while (*ip != '\0') {
					char* temp = ip;
					while (*temp != '\0' && *temp != '.') {
						++temp;
					}
					if (*temp == '.') {
						++temp;
					}
					uint64_t t = strtoul(ip, NULL, 10);
					ip = temp;
					rdata[octet++] = (uint8_t) t;
				}
				offset += 4;
			}

			count = sendto(sockfd, buffer, offset, 0, &client_addr, client_addr_len);
			if (count < 0) {
				perror("ERROR: send to client\n");
				exit(-1);
			}
			memset(buffer, 0, BUF_SIZE);
		}
    }
}