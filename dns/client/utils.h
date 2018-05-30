#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <unistd.h>
#include <memory.h>

const size_t MAX_SIZE = 256;
static const size_t VALUE = 49152;

inline void encode(unsigned char *buffer, const char *const host) {
    unsigned int current_point = 0;
    for (size_t i = 0; i < strnlen(host, MAX_SIZE) + 1; ++i) {
        if (host[i] == '.' || host[i] == 0) {
            *buffer = static_cast<unsigned char>(i - current_point);
            ++buffer;
            memcpy(buffer, host + current_point, i - current_point);
            buffer += i - current_point;
            current_point += i - current_point + 1;
        }
    }
    *buffer = 0;
}

inline void decode(unsigned char *buffer) {
    size_t i = 0;
    for (; i < strnlen(reinterpret_cast<const char *>(buffer), MAX_SIZE); i++) {
        size_t offset_size = buffer[i];
        memcpy(buffer + i, buffer + i + 1, offset_size);
        i += offset_size;
        buffer[i] = '.';
    }
    if (i > 0) {
        buffer[i - 1] = 0;
    } else {
        buffer[0] = 0;
    }
}


inline int get_name(unsigned char *reader, unsigned char *buffer, unsigned char *name) {
    int result = 1;
    int p = 0;
    char jmp = 0;
    for (; *reader; p++) {
        if (*reader >= 192) {
            reader = buffer + (*reader) * MAX_SIZE + *(reader + 1) - VALUE;
            --p;
            jmp = 1;
        } else {
            name[p] = *reader;
            reader += 1;
        }
        result += !jmp;
    }
    name[p] = 0;
    result += jmp;
    return result;
}

#endif //CLIENT_UTILS_H
