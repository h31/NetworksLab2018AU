#include "query.h"

query::query(const name &_name)
        : _name(_name)
{}

size_t query::get_size() {
    return _name.length() + 2 * sizeof(uint16_t);
}

char *query::write_to_buffer() {
    char* buffer = new char[get_size()];
    char* pointer = buffer;
    _name.to_buffer(buffer);

    uint16_t ntype = htons(type);
    memcpy(buffer, &ntype, sizeof(uint16_t));
    buffer += sizeof(uint16_t);

    uint16_t nclass = htons(_class);
    memcpy(buffer, &nclass, sizeof(uint16_t));

    return pointer;
}

query query::read_from_buffer(char *&buffer) {
    auto pointer = buffer;
    name name_ = name::from_buffer(buffer, pointer);
    query query_ = query(name_);
    memcpy(&query_.type, buffer, sizeof(uint16_t));
    query_.type = ntohs(query_.type);
    buffer += sizeof(uint16_t);
    memcpy(&query_._class, buffer, sizeof(uint16_t));
    query_._class = ntohs(query_._class);
    buffer += sizeof(uint16_t);
    return query_;
}


