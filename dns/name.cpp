#include "name.h"

size_t name::length() const {
    return _encoded.length() + 1;
}

name name::from_encoded(const std::string &encoded) {
    name new_name;
    new_name._encoded = encoded;
    new_name._decoded = decode(encoded);
    return new_name;
}

name name::from_decoded(const std::string &decoded) {
    name new_name;
    new_name._decoded = decoded;
    new_name._encoded = encode(decoded);
    return new_name;
}

name name::from_buffer(char *&buffer, char *pointer) {
    // Читает из буфера закодированное.
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

    return from_decoded(converted);
}

void name::to_buffer(char *&buffer) {
    memcpy(buffer, _encoded.c_str(), _encoded.length());
    buffer += _encoded.length();
    *buffer = '\0';
    buffer++;
}

std::string name::encode(const std::string &host) {
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
    return host_;
}

std::string name::decode(const std::string &name) {
    std::string result;
    for (size_t i = 0; i < name.length(); ++i) {
        size_t j;
        for (j = i; j < name.length() && name[j] != '.'; ++j);
        size_t count = j - i;
        result += static_cast<char>(count);
        for (; i < j; ++i) {
            result += name[i];
        }
    }
    return result;
}
