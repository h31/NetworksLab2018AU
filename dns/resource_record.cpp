#include "resource_record.h"
#include "header.h"

resource_record::resource_record(const name& _name)
        : _name(_name)
{}

size_t resource_record::get_size() {
    return 2 + 3 * sizeof(uint16_t) + sizeof(uint32_t) + rdata_length;
}

char *resource_record::write_to_buffer(size_t name_offset) {
    resource_record to_send = resource_record(_name);
    char* buffer = new char[get_size()];
    char* pointer = buffer;

    *buffer++ = static_cast<unsigned char>(name_offset / 256 + 192);
    *buffer++ = static_cast<unsigned char>(name_offset % 256);

    to_send._type = htons(_type);
    memcpy(buffer, &to_send._type, sizeof(uint16_t));
    buffer += sizeof(uint16_t);

    to_send._class = htons(_class);
    memcpy(buffer, &to_send._class, sizeof(uint16_t));
    buffer += sizeof(uint16_t);

    to_send._ttl = htonl(_ttl);
    memcpy(buffer, &to_send._ttl, sizeof(uint32_t));
    buffer += sizeof(uint32_t);

    to_send.rdata_length = htons(rdata_length);
    memcpy(buffer, &to_send.rdata_length, sizeof(uint16_t));
    buffer += sizeof(uint16_t);

    memcpy(buffer, _rdata, rdata_length);
    buffer += rdata_length;

    return pointer;
}

resource_record resource_record::read_from_buffer(char *const pointer, char*& buffer) {
    name _name = name::from_buffer(buffer, pointer);
    resource_record record = resource_record(_name);

    memcpy(&record._type, buffer, sizeof(uint16_t));
    record._type = ntohs(record._type);
    buffer += sizeof(uint16_t);

    memcpy(&record._class, buffer, sizeof(uint16_t));
    record._class = ntohs(record._class);
    buffer += sizeof(uint16_t);

    memcpy(&record._ttl, buffer, sizeof(uint32_t));
    record._ttl = ntohl(record._ttl);
    buffer += sizeof(uint32_t);

    memcpy(&record.rdata_length, buffer, sizeof(uint16_t));
    record.rdata_length = ntohs(record.rdata_length);
    buffer += sizeof(uint16_t);

    record._rdata = new char[record.rdata_length];
    for (int i = 0; i < record.rdata_length; ++i) {
        record._rdata[i] = *buffer++;
    }
    return record;
}

