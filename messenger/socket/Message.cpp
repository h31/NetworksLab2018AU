#include "Message.h"

bool Message::operator<(const Message &other) const {
    return time < other.time; // todo check if this is ok
}
