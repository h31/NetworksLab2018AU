#include "query.h"

size_t query::get_size() {
    return name.length() + 1 + 4;
}

// Конвертировать `host = vk.com` в `name = 2vk3com`
void query::set_name(const std::string &host) {
    std::string host_;

    int k = 0;
    for (auto it = host.rbegin(); it != host.rend(); it++) {
        if (*it != '.') {
            host_ = *it + host_;
            k++;
        } else {
            host_ = std::string(1, static_cast<char>(k)) + host_;
            k = 0;
        }
    }
    host_ = std::string(1, static_cast<char>(k)) + host_;
    name = host_;
}

char *query::write_to_buffer() {
    char* buffer = new char[get_size()];
    char* pointer = buffer;
    memcpy(buffer, name.c_str(), name.length());
    buffer[name.length()] = '\0';
    buffer += name.length() + 1;
    query to_send;

    to_send.type = type;
    memcpy(buffer, &to_send.type, sizeof(uint16_t));
    buffer += sizeof(uint16_t);

    to_send._class = _class;
    memcpy(buffer, &to_send._class, sizeof(uint16_t));
    return pointer;
}

query query::read_from_buffer(char *&buffer) {
    auto pointer = buffer;
    std::string qname;
    for (; *buffer != '\0'; buffer++) {
        qname += *buffer;
    }
    buffer++;
    query query_;
    query_.name = qname;
    memcpy(&query_.type, buffer, sizeof(uint16_t));
    query_.type = ntohs(query_.type);
    buffer += sizeof(uint16_t);
    memcpy(&query_._class, buffer, sizeof(uint16_t));
    query_._class = ntohs(query_._class);
    buffer += sizeof(uint16_t);
    return query_;
}

