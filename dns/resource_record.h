#ifndef DNS_CLIENT_RESOURCE_RECORD_H
#define DNS_CLIENT_RESOURCE_RECORD_H

#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <cstring>

#include "name.h"

struct resource_record {
    name _name;
    uint16_t _type;
    uint16_t _class;
    uint32_t _ttl;
    uint16_t rdata_length;
    char* _rdata;

    explicit resource_record(const name& _name);

    size_t get_size();

    char* write_to_buffer(size_t name_offset);

    static resource_record read_from_buffer(char* pointer, char*& buffer);
};


#endif //DNS_CLIENT_RESOURCE_RECORD_H

