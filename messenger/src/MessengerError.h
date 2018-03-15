#ifndef MESSENGER_MESSENGERERROR_H
#define MESSENGER_MESSENGERERROR_H

#include <stdexcept>

struct MessengerError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct ProtocolError : std::runtime_error {
    using std::runtime_error::runtime_error;
    ProtocolError(): std::runtime_error("Protocol error") {}
};

#endif //MESSENGER_MESSENGERERROR_H
