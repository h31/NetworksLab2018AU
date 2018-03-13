#include <iostream>
#include <cstring>
#include <cassert>
#include <cstddef>
#include <limits>
#include <algorithm>

#include "MessengerError.h"
#include "ElegramAll.h"

int main(int argc, char **argv) {
    int port = 5001;
    for (int i = 1; i < argc; ++i) {
        const std::string args = argv[i];
        if (std::all_of(args.cbegin(), args.cend(), [](char c) { return isdigit(c); })) {
            port = stoi(args);
        }
    }
    auto serverSocket = std::make_shared<ServerSocket>(port);
    serverSocket->listen();
    auto acceptSocket = serverSocket->accept();
    auto message = acceptSocket->read_message();
    std::cout << "Here is the message: " + message->buffer << std::endl;
    acceptSocket->write_message(std::make_shared<Message>("I got your message: " + message->buffer));
    return 0;
}
