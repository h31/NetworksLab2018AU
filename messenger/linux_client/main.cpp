#include <iostream>
#include "Client.h"

int main(int argc, char ** argv) {
    if (argc != 4) {
        std::cout << "expected 3 arguments in format: <server ip>, <server port>, <user nickname>" << std::endl;
        exit(0);
    }

    std::string address = argv[1];
    int port = atoi(argv[2]);
    std::string nickname = argv[3];

    Client client(address, port, nickname);

    while (true) {
        int c = getchar();
        if (c == 'm') {
            client.mute();
            std::string message;
            std::getline(std::cin, message);
            client.send(message);
            client.unmute();
        } else if (c == 'q') {
            break;
        }
    }
}