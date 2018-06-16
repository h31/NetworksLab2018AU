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

std::string resource_record::read_name(char* pointer, char*& buffer) {
    std::string result;
    int count = 1;
    bool jumped = false;

    unsigned char *ubuffer = reinterpret_cast<unsigned char *>(buffer);

    while (*ubuffer != '\0') {
        while (*ubuffer < 192 && *ubuffer != '\0') {
            result += *ubuffer++;
            if (!jumped) {
                count++;
            }
        }

        if (*ubuffer >= 192) {
            int offset = (*ubuffer) * 256 + *(ubuffer + 1) - 49152;
            ubuffer = reinterpret_cast<unsigned char *>(pointer) + offset;
            jumped = true;
        }
    }

    if (jumped) {
        count++;
    }

    buffer += count;

    std::string converted;

    for (size_t i = 0; i < result.size(); ) {
        if (!converted.empty()) {
            converted += ".";
        }
        int cnt = static_cast<int>(result[i]);
        i++;
        for (int j = 0; j < cnt; ++j, ++i) {
            converted += result[i];
        }
    }

    return converted;
}

resource_record resource_record::read_from_buffer(char* pointer, char*& buffer) {
    resource_record record{};
    record._name = read_name(pointer, buffer);
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

