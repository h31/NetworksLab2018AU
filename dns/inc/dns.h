#ifndef DNS_H
#define DNS_H

#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define T_A 1
#define QUERY_SIZE (1 << 16)
#define DNS_PORT 53

struct header {
    unsigned short id;

    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;

    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;

    unsigned short q_count;
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

struct question {
    unsigned short qtype;
    unsigned short qclass;
};

struct resource {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
} __attribute__((packed));


static inline int url_to_dns(unsigned char *dns, unsigned char *host) {
	int lock = 0, i;
	int host_len = (int) strlen((char *) host) + 1;

	strcat((char *) host, ".");
	for (i = 0; i < host_len; ++i) {
		if (host[i] != '.')
			continue;

		*dns++ = (unsigned char) (i - lock);
		for (; lock < i; ++lock)
			*dns++ = host[lock];
		++lock;
	}
	*dns = '\0';
	return host_len;
}

static inline
unsigned char *dns_to_url(unsigned char *reader, unsigned char *buffer, int *count) {
	unsigned char *name;
	unsigned int p = 0, jumped = 0, offset;
	int i, j;

	*count = 1;
	name = (unsigned char *) malloc(256);
	name[0] = '\0';

	while (*reader != 0) {
		if (*reader >= 192) {
			offset = (*reader) * 256 + *(reader + 1) - 49152;
			reader = buffer + offset - 1;
			jumped = 1;
		} else
			name[p++] = *reader;

		reader = reader + 1;
		if (jumped == 0)
			*count = *count + 1;
	}

	name[p] = '\0';
	if (jumped == 1)
		*count = *count + 1;

	for (i = 0; i < (int) strlen((const char *) name); ++i) {
		p = name[i];
		for (j = 0; j < (int) p; ++j, ++i)
			name[i] = name[i + 1];
		name[i] = '.';
	}
	name[i - 1] = '\0';
	return name;
}

#endif /* DNS_H */