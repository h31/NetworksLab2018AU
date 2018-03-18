#pragma once

#include <string>

struct Message {
    bool operator<(const Message & other) const;

    std::string text;
    std::string time;
    std::string nickname;
};


