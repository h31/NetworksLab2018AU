#ifndef MESSENGER_MESSAGE_H
#define MESSENGER_MESSAGE_H

#include <string>

struct Message {
    explicit Message(size_t length);
    
    explicit Message(const std::string &message);
    
    char *ptr();
    
    ssize_t size() const;

    std::string buffer;
};

#endif //MESSENGER_MESSAGE_H
