#ifndef DNS_CLIENT_DNS_PACKET_H
#define DNS_CLIENT_DNS_PACKET_H

#pragma once

#include "header.h"
#include "query.h"
#include "resource_record.h"

#include <vector>
#include <cstring>
#include <algorithm>

struct dns_packet {
    header _header;
    std::vector<query> questions;
    std::vector<resource_record> answers;
    std::vector<resource_record> authorities;
    std::vector<resource_record> additionals;

    char* write_to_buf(size_t &offset);

    static dns_packet read_from_buffer(char* buffer);

    void print_result();
};

#endif //DNS_CLIENT_DNS_PACKET_H

