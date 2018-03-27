#include <stdio.h>
#include <netdb.h>
#include <string.h>

#include "dns_header.h"

struct dns_header *parse_dns_header(const char *buf) {
    struct dns_header *header = (struct dns_header *) buf;
    header->id = ntohs(header->id);
    header->flags = ntohs(header->flags);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);
    return header;
}

size_t output_dns_header(struct dns_header* header, char *dst) {
    header->id = htons(header->id);
    header->flags = htons(header->flags);
    header->qdcount = htons(header->qdcount);
    header->ancount = htons(header->ancount);
    header->nscount = htons(header->nscount);
    header->arcount = htons(header->arcount);
    size_t header_size = sizeof(struct dns_header);
    memcpy(dst, header, header_size);
    return header_size;
}

size_t append_hostname_to_buffer(const char *hostname, char *dst) {
    size_t dst_size = 0;
    int i = 0;
    while (hostname[i] != 0) {
        int j = i;
        while (hostname[j] != '.' && hostname[j] != 0) {
            j++;
        }
        dst[dst_size++] = (char) (j - i);
        while (i != j) {
            dst[dst_size++] = hostname[i++];
        }
        if (hostname[i] == '.') {
            i++;
        }
    }
    dst[dst_size++] = 0;
    //QTYPE representing A records
    dst[dst_size++] = 0x00;
    dst[dst_size++] = 0x01;
    // QCLASS representing Internet addresses
    dst[dst_size++] = 0x00;
    dst[dst_size++] = 0x01;
    return dst_size;
}

size_t read_domain_name(const char *ptr, const char *buffer_start, char *dst) {
    if ((u_int8_t ) *ptr > 63) {
        // meaning it is a pointer
        int offset = ntohs(*((u_int16_t *) ptr));
        offset &= (1 << 14) - 1;
        read_domain_name(buffer_start + offset, buffer_start, dst);
        return 2;
    } else {
        size_t size = (u_int8_t) *ptr;
        memcpy(dst, ptr + 1, size);
        dst += size;
        if (*(ptr + 1 + size) == 0) {
            *dst = 0;
            return 2 + size;
        } else {
            *dst = '.';
            dst += 1;
            return 1 + size + read_domain_name(ptr + 1 + size, buffer_start, dst);
        }
    }
}
