#include <iostream>
#include <string>

#include "ElegramAll.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    const std::string hostname = argv[1];
    const std::string portstr = argv[2];
    auto portno = stoi(portstr);
    SocketWrapper socket = std::make_shared<Socket>(hostname, portno);
    std::cout << "Please enter the message: " << std::endl;
    std::string line;
    std::getline(std::cin, line);
    auto input_message = std::make_shared<Message>(line);
    socket->write_message(input_message);
    auto output_message = socket->read_message();
    std::cout << "Received message: " + output_message->buffer << std::endl;
    return 0;
}
