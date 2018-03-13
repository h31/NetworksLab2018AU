#include "Message.h"
#include "MessengerError.h"

Message::Message(size_t length)
        : buffer(length, '\0')
{}

Message::Message(const std::string &message)
        : buffer(message)
{}

char *Message::ptr() {
    if (size() <= 0) {
        throw MessengerError("Buffer not initialized");
    }
    return &buffer[0];
}

ssize_t Message::size() const {
    return buffer.size();
}
