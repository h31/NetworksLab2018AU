#ifndef DNS_CLIENT_NAME_H
#define DNS_CLIENT_NAME_H

#pragma once

#include <string>
#include <cstring>

struct name {
    std::string _decoded;
    std::string _encoded;

    size_t length() const;

    static name from_encoded(const std::string& encoded);

    static name from_decoded(const std::string& decoded);

    static name from_buffer(char*& buffer, char *pointer);

    void to_buffer(char*& buffer);

    // Конвертировать `vk.com` в `2vk3com`
    static std::string encode(const std::string& host);

    // Конвертировать `2vk3com` в `vk.com`.
    static std::string decode(const std::string& name);

private:
    name() = default;
};

#endif //DNS_CLIENT_NAME_H

