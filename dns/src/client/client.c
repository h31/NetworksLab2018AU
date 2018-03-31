#include<stdio.h>

#include<sys/socket.h>
#include<arpa/inet.h>

#include "dns.h"

static int fill_query(unsigned char *host, unsigned char buf[QUERY_SIZE])
{
	struct header *dns;
	struct question *question;
	unsigned char *name;
	int name_len;

	dns = (struct header *)buf;
	memset(dns, 0, sizeof *dns);
	dns->id = (unsigned short) htons(getpid());
	dns->rd = 1;
	dns->q_count = htons(1);

	name = &buf[sizeof *dns];
	name_len = url_to_dns(name, host) + 1;

	question = (struct question *)(name + name_len);
	question->qtype = htons(T_A);
	question->qclass = htons(1);

	return sizeof(*dns) + sizeof(*question) + name_len;
}

static void process_response(unsigned char *buf, int request_len)
{
	struct resource *res;
	struct sockaddr_in addr = {};
	unsigned char *reader = &buf[request_len];
	struct header *dns = (struct header *)buf;

	if (dns->ans_count == 0) {
		printf("domain doesn't found\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < ntohs(dns->ans_count); ++i) {
		int count = 0;
		unsigned char *name;

		name = dns_to_url(reader, buf, &count);
		reader += count;
		res = (struct resource *)reader;
		reader += sizeof(*res);
		free(name);

		if(ntohs(res->type) == T_A) {
			addr.sin_addr.s_addr = *(in_addr_t *)reader;
			printf("%s", inet_ntoa(addr.sin_addr));

			return;
		} else {
			free(dns_to_url(reader, buf, &count));
			reader += count;
		}
	}

	fprintf(stderr, "ipv4 not found\n");
	exit(EXIT_FAILURE);
}

static int get_dns_record(unsigned char *host, unsigned char buf[QUERY_SIZE], struct sockaddr_in *dest)
{
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int query_len = fill_query(host, buf);
	socklen_t sl;

	if (sendto(sock, (char*)buf, (size_t)query_len, 0, (struct sockaddr*)dest, sizeof(*dest)) < 0)
		exit(EXIT_FAILURE);

	sl = sizeof *dest;
	if (recvfrom(sock, (char*)buf, QUERY_SIZE, 0, (struct sockaddr*)dest, &sl) < 0)
		exit(EXIT_FAILURE);

	return query_len;
}

static void get_dns_addr(struct sockaddr_in *dest, const char *ip)
{
	dest->sin_family = AF_INET;
	dest->sin_port = htons(DNS_PORT);
	dest->sin_addr.s_addr = inet_addr(ip);
}

int main(int argc , char *argv[])
{
	unsigned char buffer[QUERY_SIZE] = {};
	unsigned char hostname[1000];
	char dns[1000];
	struct sockaddr_in dns_addr = {};
	int offset;

	printf("hostname, dns_ip:\n");
	scanf("%s" , hostname);
	scanf("%s" , dns);

	get_dns_addr(&dns_addr, dns);
	offset = get_dns_record(hostname, buffer, &dns_addr);
	process_response(buffer, offset);

	return 0;
}