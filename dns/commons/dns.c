#include "dns.h"

int read_row_domen(char *domen, char *buffer) {
    int domen_pos = 0, buffer_pos = 0, n;
    while ((n = buffer[buffer_pos++])) {
        if (domen_pos != 0) {
            domen[domen_pos++] = '.';
        }
        memcpy(domen + domen_pos, buffer + buffer_pos, n);
        buffer_pos += n;
        domen_pos += n;
    }
    domen[domen_pos] = 0;
    return buffer_pos;
}

uint16_t read_uint16_t_from_buffer(char *buffer) {
    return (buffer[0] << 8) + buffer[1];
}

uint32_t read_uint32_t_from_buffer(char *buffer) {
    return (read_uint16_t_from_buffer(buffer) << 16) + read_uint16_t_from_buffer(buffer + 2);
}

void write_uint16_t_to_buffer(char *buffer, uint16_t value) {
    buffer[0] = value >> 8;
    buffer[1] = value & ((1 << 8) - 1);
}

void write_uint32_t_to_buffer(char *buffer, uint32_t value) {
    write_uint16_t_to_buffer(buffer, value >> 16);
    write_uint16_t_to_buffer(buffer + 2, value & ((1 << 16) - 1));
}