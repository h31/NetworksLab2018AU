#include <iostream>
#include <string>

#include "ElegramAll.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port username\n", argv[0]);
        exit(0);
    }

    const std::string hostname = argv[1];
    const std::string portstr = argv[2];
    const std::string username = argv[3];
    auto portno = stoi(portstr);
    auto socket = std::make_shared<Socket>(hostname, portno, username);
    std::cout << "Please enter the message: " << std::endl;
    std::string line;
    std::getline(std::cin, line);
//    auto input_message = std::make_shared<Message>(line, username, Date::now());
    socket->write_message(line, Date::now());
    auto message_type = static_cast<MessageType>(socket->read_uint());
    if (message_type != MessageType::MESSAGE) {
        throw ProtocolError("Expecting answer from server.");
    }
    auto output_message = socket->read_message();
    std::cout << "Received message: " + output_message->buffer << std::endl;
    
    socket->finish();
    return 0;
}
