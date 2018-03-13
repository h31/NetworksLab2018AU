#ifndef MESSENGER_MESSENGERERROR_H
#define MESSENGER_MESSENGERERROR_H

#include <stdexcept>

struct MessengerError : std::runtime_error {
//    explicit MessengerError(const std::string &message)
//            : std::runtime_error(message)
//    {}
    
    using std::runtime_error::runtime_error;
};

#endif //MESSENGER_MESSENGERERROR_H
