#ifndef PACKET
#define PACKET

#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#define BUF_SIZE 65536
#define T_A 1

struct header {
        uint16_t _id;
        uint16_t _flags;
        uint16_t t_questions;
        uint16_t t_answer_rrs;
        uint16_t t_authority_rrs;
        uint16_t t_additional_rrs;

        char* write_to_buffer();

        static header read_from_buffer(char*& buf);
};

#endif // PACKET

