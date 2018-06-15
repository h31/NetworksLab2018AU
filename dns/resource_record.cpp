#include "resource_record.h"

size_t resource_record::get_size() {
    return 2 + 4 * sizeof(uint16_t) + sizeof(uint32_t) + rdata_length;
}

char *resource_record::write_to_buffer() {
    char* buffer = new char[get_size()];
    buffer[0] = 1;
    buffer[1] = 1;
    buffer += 2;
    resource_record to_send = resource_record();
    to_send._name = htons(_name);
    memcpy(buffer, &to_send._name, sizeof(uint16_t));
    buffer += sizeof(uint16_t);
    to_send._type = htons(_type);
    memcpy(buffer, &to_send._type, sizeof(uint16_t));
    to_send._class = htons(_class);
    buffer += sizeof(uint16_t);
    memcpy(buffer, &to_send._class, sizeof(uint16_t));
    to_send._ttl = htonl(_ttl);
    buffer += sizeof(uint16_t);
    memcpy(buffer, &to_send._ttl, sizeof(uint32_t));
    to_send.rdata_length = htons(rdata_length);
    buffer += sizeof(uint32_t);
    memcpy(buffer, &to_send.rdata_length, sizeof(uint16_t));
    buffer += sizeof(uint16_t);
    memcpy(buffer, &_rdata, rdata_length);
    return buffer;
}

std::string resource_record::read_name(char *buffer) {
    int current = _name;
    std::string result;
    while (buffer[current] != '\0') {
        if (result != "") {
            result += ".";
        }
        if (buffer[current]) {
            int count = static_cast<int>(buffer[current++]);
            for (int i = 0; i < count; i++) {
                result += buffer[current++];
            }
        }
    }

    return result;
}

resource_record resource_record::read_from_buffer(char*& buffer) {
    buffer += 2;
    resource_record record{};
    memcpy(&record._name, buffer, sizeof(uint16_t));
    record._name = ntohs(record._name);
    buffer += sizeof(uint16_t);
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
        record._rdata[i] = (*buffer)++;
    }
    return record;
}
