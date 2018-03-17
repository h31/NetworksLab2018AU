#ifndef MESSENGER_ELEGRAMALL_H
#define MESSENGER_ELEGRAMALL_H

#include "ElegramFwd.h"
#include "MessengerError.h"
#include "Message.h"
#include "ServerSocket.h"
#include "Socket.h"

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

#endif //MESSENGER_ELEGRAMALL_H
