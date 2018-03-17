#include "Message.h"

bool Message::operator<(const Message &other) {
    return time < other.time; // todo check if this is ok
}
