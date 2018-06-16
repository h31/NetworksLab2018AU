#ifndef DNS_CLIENT_RESOURCE_RECORD_H
#define DNS_CLIENT_RESOURCE_RECORD_H

#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <cstring>

struct resource_record {
    std::string _name;
    uint16_t _type;
    uint16_t _class;
    uint32_t _ttl;
    uint16_t rdata_length;
    char* _rdata;

    size_t get_size();

    static std::string read_name(char* pointer, char*& buffer);

    char* write_to_buffer();

    static resource_record read_from_buffer(char* pointer, char*& buffer);
};


#endif //DNS_CLIENT_RESOURCE_RECORD_H
