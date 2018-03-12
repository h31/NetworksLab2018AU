#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include "vector.h"
#include <WinSock2.h>

typedef struct {
    vector_t name;
    SOCKET sock;
	enum {true, false} is_closed;
    CONDITION_VARIABLE msg_can_consume;
    CRITICAL_SECTION msg_section;
} client_t;

static void free_client(client_t* client) {
	free_vector(&client->name);
}

#endif //CLIENT_H
