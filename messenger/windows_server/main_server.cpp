#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Messages.h"
#include "MessageSender.h"
#include "ClientHandler.h"
#include "Server.h"

uint16_t SERVER_PORT = 5001;

int main(int argc, char *argv[]) {
    std::thread server_ui();
    auto * server = new Server(SERVER_PORT, 1000);
    (*server).run();
    delete server;
    return 0;
}


void read_input(Server * server) {
    std::string input;
	getline(std::cin, input);
    if (input == "stop") {
        (*server).stop();
    } else {
        std::cout << "Usage:\n stop - stop the server" << std::endl;
    }
}