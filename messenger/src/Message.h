#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H

#include "Date.h"
#include <string>

struct Message {
    Message(const std::string &message, const std::string &username, const Date &date);
    
//    Message(const std::string &message, const Date &date);

    char *ptr();
    
    ssize_t size() const;
    
    std::string to_string() const;

    std::string buffer;
    std::string username; // from.
    Date date;
};

#endif //MESSENGER_MESSAGE_H
