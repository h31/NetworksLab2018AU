#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <memory.h>
#include <message.h>
#include <unistd.h>

#define BUF_SIZE 65536
#define RESOURCE_NAME_SIZE 256

uint8_t buffer[BUF_SIZE];
uint8_t resource_name[RESOURCE_NAME_SIZE];

void get_options(int argc, char** argv, char** dns_server_addr, uint16_t* dns_server_port);

uint8_t is_pointer(uint8_t ch) {
	if (((ch & 0xc0) >> 6) == 3) {
		return 1;
	}
	return 0;
}

uint16_t get_offset(const uint8_t* pointer) {
	return (uint16_t) ((*pointer * 256 + *(pointer + 1)) & 0x3fff);
}

uint8_t* fill_resource_name(uint8_t* resource_name_ptr, uint8_t* resource_name) {
	if (is_pointer(*resource_name_ptr)) {
		uint16_t offset = get_offset(resource_name_ptr);
		fill_resource_name(buffer + offset, resource_name);
		return resource_name_ptr + 2;
	}
	while (1) {
		uint8_t label = (uint8_t) *resource_name_ptr;
		if (label == 0) {
			*resource_name = 0;
			return resource_name_ptr + 1;
		}
		if (is_pointer(label)) {
			uint16_t offset = get_offset(resource_name_ptr);
			fill_resource_name(buffer + offset, resource_name);
			return resource_name_ptr + 2;
		}
		++resource_name_ptr;
		for (uint8_t i = 0; i < label; ++i) {
			*resource_name = *resource_name_ptr;
			resource_name++;
			resource_name_ptr++;
		}
		*resource_name = '.';
		++resource_name;
	}
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "usage: -a <dns-server-addr> -p <dns-server-port>");
        exit(1);
    }
    uint16_t dns_server_port;
    char *dns_server_addr;
    get_options(argc, argv, &dns_server_addr, &dns_server_port);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    char server_port[6];
    snprintf(server_port, 6, "%u", dns_server_port);

    if (getaddrinfo(dns_server_addr, server_port, &hints, &res) != 0) {
        perror("ERROR: getaddrinfo failed");
        return 1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    while (1) {
        char *name = NULL;
        size_t name_length = 0;
        printf("input domain name: ");
        ssize_t count = getline(&name, &name_length, stdin);
        if (count < 0) {
            perror("ERROR: Cannot read domain name");
            exit(-1);
        }
        // remove newline
        --count;
        name[count] = '\0';

		header_t* header = (header_t*) buffer;
		header->id = htons((uint16_t) getpid());
		header->qr = 0;
		header->opcode = 0;
		header->rd = 0;
		header->z = 0;
		header->qdcount = htons(1);
		header->ancount = 0;
		header->nscount = 0;
		header->arcount = 0;

		uint8_t* qname = buffer + sizeof(header_t);
        uint8_t pos = 0;
        uint8_t index = 0;
        for (index = 0; index < count; ++index) {
            if (name[index] == '.') {
                qname[pos] = index - pos;
                pos = index + (uint8_t) 1;
            } else {
                qname[index + 1] = (uint8_t)name[index];
            }
        }
        qname[pos] = index - pos;
        qname[index + 1] = 0;

		question_t* question = (question_t*) (buffer + sizeof(header_t) + strlen(qname) + 1);
        question->class = htons(CLASS_IN);
        question->type = htons(TYPE_A);

        ssize_t n = sendto(sockfd, buffer,
						   sizeof(header_t) + strlen((const char *) qname) + 1 + sizeof(question_t), 0,
						   res->ai_addr, res->ai_addrlen);
        if (n < 0) {
            perror("failed to send header to server\n");
            exit(-1);
        }

		memset(buffer, 0, BUF_SIZE);
        struct sockaddr from_sock;
        socklen_t socklen = sizeof(from_sock);
        n = recvfrom(sockfd, buffer, BUF_SIZE, 0, &from_sock, &socklen);
        if (n < 0) {
            perror("failed to receive header from server\n");
            continue;
        }

		printf("HEADER:\n\tQUERY: %d, ANSWER: %d, AUTHORITY: %d, ADDITIONAL: %d\n",
				ntohs(header->qdcount), ntohs(header->ancount),
			    ntohs(header->nscount), ntohs(header->arcount));
		printf("QUESTION SECTION:\n\t%s\tIN\tA\n", name);

		if (ntohs(header->ancount) > 0) {
			printf("ANSWER SECTION:\n");
			uint8_t* offset = (uint8_t*)(buffer + sizeof(header_t)
										 + strlen((const char*) qname) + 1
										 + sizeof(question_t));
			for (uint32_t i = 0; i < ntohs(header->ancount); ++i) {
				offset = fill_resource_name(offset, resource_name);
				answer_t* answer = (answer_t*) offset;
				// we process only such records
				if (answer->class == htons(CLASS_IN) && answer->type == htons(TYPE_A)) {
					printf("\t%s\t\t", resource_name);
					uint8_t* ip = (uint8_t*) answer + sizeof(answer_t);
					for (uint8_t j = 0; j < ntohs(answer->rdlength) - 1; ++j) {
						printf("%u.", *ip++);
					}
					printf("%u\n", *ip);
				}
				uint32_t temp = sizeof(answer_t) + ntohs(answer->rdlength);
				offset += temp;
			}
		}
		printf("===========================================\n");
    }
}