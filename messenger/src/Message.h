#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H

#include <iosfwd>
#include "Date.h"
#include <string>

struct Message {
    static const Message &invalid_message() {
        static const Message msg{"", "", Date::invalid()};
        return msg;
    }
    Message(const std::string &message, const std::string &username, const Date &date);
    
//    Message(Message &&) = default;
//
//    Message(const Message &) = default;
//
//    Message &operator=(const Message &rhs) {
//        buffer = rhs.buffer;
//        username = rhs.username;
//        date = rhs.date;
//        return *this;
//    }

    char *ptr();
    
    ssize_t size() const;
    
    std::string to_string() const;
    
    friend bool operator<(const Message &lhs, const Message &rhs);
    
    friend bool operator==(const Message &lhs, const Message &rhs);
    
    friend bool operator!=(const Message &lhs, const Message &rhs);
    
    friend std::ostream &operator<<(std::ostream &os, const Message &lhs);

    std::string buffer;
    std::string username; // from.
    Date date;
};

#endif //MESSENGER_MESSAGE_H
