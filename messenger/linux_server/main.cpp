#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

#include <string.h>
#include <utility>
#include <vector>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../MessagesQueue.h"
#include "MessageSender.h"
#include "ClientHandler.h"
#include "Server.h"

uint16_t SERVER_PORT = 5001;

int main(int argc, char *argv[]) {
    Server server(SERVER_PORT, 1000);
    server.run();
    return 0;
}