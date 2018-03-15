#include <sstream>
#include "Message.h"
#include "MessengerError.h"

Message::Message(const std::string &message, const std::string &username, const Date &date)
        : buffer(message)
        , username(username)
        , date(date)
{}

//Message::Message(const std::string &message, const Date &date)
//        : buffer(message)
//        , date(date)
//{}

char *Message::ptr() {
    if (size() <= 0) {
        throw MessengerError("Buffer not initialized");
    }
    return &buffer[0];
}

ssize_t Message::size() const {
    return buffer.size();
}

std::string Message::to_string() const {
    std::ostringstream os;
    os << username << ": " << date.pretty_string() << ": " << buffer;
    return os.str();
}
