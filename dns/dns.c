#include <stdint.h>
#include "dns.h"

int write16(char* buffer, uint16_t x) {
    buffer[0] = (char) ((x >> 8) & 0xff);
    buffer[1] = (char) (x & 0xff);
    return 2;
}

int write32(char* buffer, uint32_t x) {
    buffer[0] = (char) ((x >> 24) & 0xff);
    buffer[1] = (char) ((x >> 16) & 0xff);
    buffer[2] = (char) ((x >> 8) & 0xff);
    buffer[3] = (char) (x & 0xff);
    return 4;
}

uint16_t read16(const char* buffer) {
    return (uint16_t) ((buffer[0] << 8) | buffer[1]);
}
