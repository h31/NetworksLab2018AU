#pragma once

#include <string>

struct Message {
    inline bool operator<(const Message & other);

    std::string text;
    std::string time;
    std::string nickname;
};


