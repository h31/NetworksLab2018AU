#ifndef DNS_CLIENT_QUERY_H
#define DNS_CLIENT_QUERY_H

#include <cstdint>
#include <string>
#include <cstring>
#include <algorithm>
#include <netinet/in.h>

#include "name.h"

struct query {
    name _name;
    uint16_t type;
    uint16_t _class;

    explicit query(const name &_name);

    size_t get_size();

    char* write_to_buffer();

    static query read_from_buffer(char *&buffer);
};


#endif //DNS_CLIENT_QUERY_H

