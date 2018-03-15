#include <iostream>
#include <iomanip>
#include <sstream>
#include "Message.h"
#include "MessengerError.h"

Message::Message(const std::string &message, const std::string &username, const Date &date)
        : buffer(message)
        , username(username)
        , date(date)
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

std::string Message::to_string() const {
    std::ostringstream os;
    os << username << ": " << date.pretty_string() << ": " << buffer;
    return os.str();
}

bool operator<(const Message &lhs, const Message &rhs) {
    return lhs.date < rhs.date;
}

std::ostream &operator<<(std::ostream &os, const Message &lhs) {    
    auto &date = lhs.date;
    os << "<" << std::setfill('0') << std::setw(2) << date.hours() << ":";
    os << std::setfill('0') << std::setw(2) << date.minutes()      << ":";
    os << std::setfill('0') << std::setw(2) << date.seconds() << "> ";
    os << "[" << lhs.username << "] ";
    os << lhs.buffer;
    return os;
}
