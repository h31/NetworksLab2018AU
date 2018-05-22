#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include <stdint.h>

// we consider only these type and class
#define CLASS_IN 1
#define TYPE_A 1

typedef struct {
    uint16_t id;
    uint8_t qr:1;
    uint8_t opcode:4;
    uint8_t aa:1;
    uint8_t tc:1;
    uint8_t rd:1;
    uint8_t ra:1;
    uint8_t z:3;
    uint8_t rcode:4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} header_t;


typedef struct {
    uint16_t type;
    uint16_t class;
} question_t;


typedef struct __attribute__((__packed__)) {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
} answer_t;

#endif //CLIENT_MESSAGE_H
