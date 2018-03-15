#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H

#include <iosfwd>
#include "Date.h"
#include <string>

struct Message {
    Message(const std::string &message, const std::string &username, const Date &date);
    
    Message(Message &&) = default;

    char *ptr();
    
    ssize_t size() const;
    
    std::string to_string() const;
    
    friend bool operator<(const Message &lhs, const Message &rhs);
    
    friend std::ostream &operator<<(std::ostream &os, const Message &lhs);

    std::string buffer;
    std::string username; // from.
    Date date;
};

#endif //MESSENGER_MESSAGE_H
