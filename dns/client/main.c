#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dns_protocol.h"

#define BUFFER_SIZE 65536
static const unsigned short PORT = 53; 

static void assert_error(char expression, const char *msg)
{
    if (!expression)
    {
        perror(msg);
        exit(1);
    }
}

// This will convert www.google.com to 3www6google3com 
static void to_dns_name(unsigned char *dns, const char * const host)
{
	unsigned int lock = 0;
	for (unsigned int i = 0; i < strlen(host) + 1; i++)
	{
		if (host[i] == '.' || host[i] == 0)
		{
			*dns++ = i - lock;		
			memcpy(dns, host + lock, i - lock); 
            dns += i - lock;
			lock += i - lock + 1;
		} 
	} 
	*dns = 0;
} 

static unsigned char *read_name(unsigned char* reader, unsigned char *buffer, int *count)
{
    *count = 1;
    unsigned char *name = (unsigned char*) malloc(256);
    int p = 0;
    char jumped = 0;
    for (; *reader; p++)
    {
        if (*reader >= 192)
        {
            const int offset = (*reader) * 256 + *(reader + 1) - 49152;
            reader = buffer + offset;
            --p;
            jumped = 1;
        }
        else
        {
            name[p] = *reader;
            reader += 1;
        }

        *count += !jumped;
    }
    name[p] = 0;
    *count += jumped;

    unsigned int i = 0; 
    for (; i < strlen((char*)name); i++)
    {
        p = name[i];
        memcpy(name + i, name + i + 1, p);
        i += p;
        name[i] = '.';
    }
    name[i > 0 ? i - 1 : 0] = 0;
    return name;
}

int main(int argc, char **argv)
{
    assert_error(argc == 3, "ERROR: Invalid count arguments\n Usage: dns_client dns_server resolve_address ");
    const char * const dns_server = argv[1];
    const char * const resolve_address = argv[2];

    const int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);  
    assert_error(client_socket_fd >= 0, "ERROR: opening socket");

	struct sockaddr_in serveraddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr(dns_server),
		.sin_port = htons(PORT)
	};

    unsigned char buffer[BUFFER_SIZE] = { };
    struct DNS_HEADER *dns = (struct DNS_HEADER*)buffer;
    dns->id = (unsigned short) htons(getpid());
    dns->q_count = htons(1); //we have only 1 question
	unsigned char *qname =	(unsigned char*)(buffer + sizeof(struct DNS_HEADER));
	to_dns_name(qname, resolve_address);
	struct QUESTION *qinfo =(struct QUESTION*)(buffer + sizeof(struct DNS_HEADER) + strlen((char*)qname) + 1);
	qinfo->qtype = htons(T_A); 
	qinfo->qclass = htons(1); // internet

	printf("Sending packet...\n");
	assert_error(sendto(client_socket_fd, (char*)buffer,
						sizeof(struct DNS_HEADER) + strlen((const char*)qname) + 1 + sizeof(struct QUESTION),
						0, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) >= 0, "Send to failes"); 
	printf("Done!\n");

	unsigned int serverlen = sizeof serveraddr;
	
    bzero(buffer, BUFFER_SIZE);
	printf("Receiving answer...\n");
    assert_error(recvfrom(client_socket_fd, (char*)buffer, BUFFER_SIZE , 0 , (struct sockaddr*)&serveraddr , (socklen_t*)&serverlen) >= 0, "recvfrom failed");
    printf("Done\n");

	printf("The response contains:\n");
    printf(" %d Questions.\n",ntohs(dns->q_count));
    printf(" %d Answers.\n",ntohs(dns->ans_count));
    printf(" %d Authoritative Servers.\n",ntohs(dns->auth_count));
    printf(" %d Additional records.\n\n",ntohs(dns->add_count));

    printf("Answer Records: %d \n" , ntohs(dns->ans_count));
    unsigned char* reader = buffer + sizeof(struct DNS_HEADER) + strlen((const char*)qname) + 1 + sizeof(struct QUESTION);
    for (int i = 0; i < ntohs(dns->ans_count); i++)
    {
        int offset = 0;
        unsigned char* name = read_name(reader, buffer, &offset);
        printf(" Name: %s ", name); 
        reader += offset;
        struct R_DATA* data = (struct R_DATA*)reader;
        reader += sizeof(struct R_DATA);
        if (ntohs(data->type) == T_A)
        {
            unsigned char* rdata = (unsigned char*)malloc(ntohs(data->data_len));
            memcpy(rdata, reader, ntohs(data->data_len));
            rdata[ntohs(data->data_len)] = 0;
            reader += ntohs(data->data_len);
            struct sockaddr_in a = {
                .sin_addr.s_addr = *(int*)rdata
            };
            printf("has IPv4 address: %s ", inet_ntoa(a.sin_addr));
            free(rdata);
        }
        else
        {
            int offset = 0;
            unsigned char * rdata = read_name(reader, buffer, &offset);
            reader = reader + offset;
            if (ntohs(data->type) == 5)
            {
                printf("has alias name: %s ", rdata);
            }
            free(rdata);
        }
        free(name);
		printf("\n");
    }
	
    return 0;
}
