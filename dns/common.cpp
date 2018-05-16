#include <iostream>
#include "common.h"
#define DOMAIN_READ_MASK (uint16_t)((1 << 7) + (1 << 6))

int readDomain(char *domain, uint8_t *buffer) {
    int domain_pos = 0, buffer_pos = 0;
    while (buffer[buffer_pos]) {
        if (domain_pos != 0) {
            domain[domain_pos] = '.';
            domain_pos++;
        }
        memcpy(domain + domain_pos, buffer + buffer_pos + 1, buffer[buffer_pos]);
        domain_pos += buffer[buffer_pos];
        buffer_pos += buffer[buffer_pos];
        buffer_pos++;
    }
    domain[domain_pos] = 0;
    return buffer_pos + 1;
}

uint16_t readUint16(uint8_t *buffer) {
    return (((uint16_t)buffer[0]) << 8) + buffer[1];
}

uint32_t readUint32(uint8_t *buffer) {
    return (((uint32_t) readUint16(buffer)) << 16) + readUint16(buffer + 2);
}

int writeDomain(uint8_t *buffer, char *domain) {
    int len = strlen(domain);
    int buffer_pos = 1;
    int prev_dot = -1;
    for (int domain_pos = 0; domain_pos <= len; domain_pos++) {
        if (domain_pos == len || domain[domain_pos] == '.') {
            int word_len = domain_pos - 1 - prev_dot;
            buffer[buffer_pos - word_len - 1] = word_len;
            prev_dot = domain_pos;
        } else {
            buffer[buffer_pos] = domain[domain_pos];
        }
        buffer_pos++;
    }
    buffer[buffer_pos - 1] = 0;
    return buffer_pos;
}

void writeUint16(uint8_t *buffer, uint16_t value) {
    buffer[0] = value >> 8;
    buffer[1] = value & ((1 << 8) - 1);
}

void writeUint32(uint8_t *buffer, uint32_t value) {
    writeUint16(buffer, value >> 16);
    writeUint16(buffer + 2, value & ((1 << 16) - 1));
}

int readDnsRequest(DnsRequest &dnsRequest, uint8_t *buffer) {
    bzero(dnsRequest.query->domain, MAX_MSG_LEN);
    dnsRequest.header->id = readUint16(buffer);
    dnsRequest.header->flags = readUint16(buffer + 2);
    dnsRequest.header->questions = readUint16(buffer + 4);
    dnsRequest.header->answerRRs = readUint16(buffer + 6);
    dnsRequest.header->autorityRRs = readUint16(buffer + 8);
    dnsRequest.header->additionalRRs = readUint16(buffer + 10);
    int n = readDomain(dnsRequest.query->domain, buffer + 12);
    dnsRequest.query->type = readUint16(buffer + 12 + n);
    dnsRequest.query->clazz = readUint16(buffer + 14 + n);
    return 16 + n;
}

int readDnsResponse(DnsResponse &dnsResponse, uint8_t *buffer) {
    bzero(dnsResponse.query->domain, MAX_MSG_LEN);
    dnsResponse.header->id = readUint16(buffer);
    dnsResponse.header->flags = readUint16(buffer + 2);
    dnsResponse.header->questions = readUint16(buffer + 4);
    dnsResponse.header->answerRRs = readUint16(buffer + 6);
    dnsResponse.header->autorityRRs = readUint16(buffer + 8);
    dnsResponse.header->additionalRRs = readUint16(buffer + 10);
    int n = readDomain(dnsResponse.query->domain, buffer + 12);
    dnsResponse.query->type = readUint16(buffer + 12 + n);
    dnsResponse.query->clazz = readUint16(buffer + 14 + n);
    if ((buffer[16 + n] & DOMAIN_READ_MASK) == DOMAIN_READ_MASK) {
        readDomain(dnsResponse.name, buffer + (readUint16(buffer + 16 + n) - (DOMAIN_READ_MASK << 8)));
        n += 2;
    } else {
        n += readDomain(dnsResponse.name, buffer + 16 + n);
    }
    dnsResponse.type = readUint16(buffer + 16 + n);
    dnsResponse.clazz = readUint16(buffer + 18 + n);
    dnsResponse.ttl = readUint32(buffer + 20 + n);
    dnsResponse.dataLen = readUint16(buffer + 24 + n);
    for (int i = 0; i < 4; i++) {
        dnsResponse.ip[i] = buffer[26 + n + i];
    }
    return 30 + n;
}

int writeDnsRequest(DnsRequest &dnsRequest, uint8_t *buffer) {
    writeUint16(buffer, dnsRequest.header->id);
    writeUint16(buffer + 2, dnsRequest.header->flags);
    writeUint16(buffer + 4, dnsRequest.header->questions);
    writeUint16(buffer + 6, dnsRequest.header->answerRRs);
    writeUint16(buffer + 8, dnsRequest.header->autorityRRs);
    writeUint16(buffer + 10, dnsRequest.header->additionalRRs);
    int domain_record_len = writeDomain(buffer + 12, dnsRequest.query->domain);
    writeUint16(buffer + 12 + domain_record_len, dnsRequest.query->type);
    writeUint16(buffer + 14 + domain_record_len, dnsRequest.query->clazz);
    return 16 + domain_record_len;
}

int writeDnsResponse(DnsResponse &dnsResponse, uint8_t *buffer) {
    writeUint16(buffer, dnsResponse.header->id);
    writeUint16(buffer + 2, dnsResponse.header->flags);
    writeUint16(buffer + 4, dnsResponse.header->questions);
    writeUint16(buffer + 6, dnsResponse.header->answerRRs);
    writeUint16(buffer + 8, dnsResponse.header->autorityRRs);
    writeUint16(buffer + 10, dnsResponse.header->additionalRRs);
    int n = writeDomain(buffer + 12, dnsResponse.query->domain);
    writeUint16(buffer + 12 + n, dnsResponse.query->type);
    writeUint16(buffer + 14 + n, dnsResponse.query->clazz);
    if (strcmp(dnsResponse.query->domain, dnsResponse.name) == 0) {
        writeUint16(buffer + 16 + n, (DOMAIN_READ_MASK << 8) + 12);
        n += 2;
    } else {
        n += writeDomain(buffer + 16 + n, dnsResponse.name);
    }
    writeUint16(buffer + 16 + n, dnsResponse.type);
    writeUint16(buffer + 18 + n, dnsResponse.clazz);
    writeUint32(buffer + 20 + n, TTL);
    writeUint16(buffer + 24 + n, dnsResponse.dataLen);
    for (int i = 0; i < 4; i++) {
        buffer[26 + n + i] = dnsResponse.ip[i];
    }
    return 30 + n;
}