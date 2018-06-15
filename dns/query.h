#ifndef DNS_CLIENT_QUERY_H
#define DNS_CLIENT_QUERY_H

#include <cstdint>
#include <string>
#include <cstring>
#include <algorithm>
 #include <netinet/in.h>

struct query {
    std::string name;
    uint16_t type;
    uint16_t _class;

    size_t get_size();

    void set_name(const std::string& host);

    char* write_to_buffer();

    static query read_from_buffer(char *&buffer);
};


#endif //DNS_CLIENT_QUERY_H
