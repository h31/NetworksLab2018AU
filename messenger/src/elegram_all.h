#ifndef MESSENGER_ELEGRAMALL_H
#define MESSENGER_ELEGRAMALL_H

#include "elegram_fwd.h"
#include "messenger_error.h"
#include "message.h"
#include "server_socket.h"
#include "socket.h"

#if _WIN32
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#ifdef max
#undef max
#endif

#include <limits>

static inline std::uint16_t check_and_cast_uint16(std::int64_t num) {
    if (num > std::numeric_limits<std::uint16_t>::max() || num < 0) {
        throw MessengerError("Overflow error");
    }
    return static_cast<std::uint16_t>(num);
}

#endif //MESSENGER_ELEGRAMALL_H
