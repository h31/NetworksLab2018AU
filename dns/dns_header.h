#ifndef DNS_DNS_PROTOCOL_H
#define DNS_DNS_PROTOCOL_H

#define PACKET_SIZE 1024
#define DOMAIN_BUFFER_SIZE 256

struct dns_header  {
  u_int16_t id;
  u_int16_t flags;
  u_int16_t qdcount;
  u_int16_t ancount;
  u_int16_t nscount;
  u_int16_t arcount;
};

struct dns_header *parse_dns_header(const char * buf);
size_t output_dns_header(struct dns_header* header, char *dst);

size_t append_hostname_to_buffer(const char *hostname, char *dst);

size_t read_domain_name(const char *ptr, const char *buffer_start, char *dst);

#endif //DNS_DNS_PROTOCOL_H
