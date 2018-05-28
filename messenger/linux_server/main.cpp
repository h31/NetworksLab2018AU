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
#include "../Messages.h"
#include "MessageSender.h"
#include "ClientHandler.h"
#include "Server.h"

int main(int argc, char *argv[]) {
    std::thread server_ui();
    int server_port = atoi(argv[1]);
    auto * server = new Server(server_port, 1000);
    (*server).run();
    delete server;
    return 0;
}


void read_input(Server * server) {
    std::string input;
    std::cin >> input;
    if (input == "stop") {
        (*server).stop();
    } else {
        std::cout << "Usage:\n stop - stop the server" << std::endl;
    }
}