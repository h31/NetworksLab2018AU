#ifndef DNS_ERROR_H
#define DNS_ERROR_H

#include <stdexcept>

struct DnsError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct ProtocolError : std::runtime_error {
    using std::runtime_error::runtime_error;
    ProtocolError(): std::runtime_error("Protocol error") {}
};

#endif //DNS_ERROR_H
