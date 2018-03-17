#include <iostream>
#include "Client.h"

Client::Client(const std::string &address, uint16_t port, const std::string &nickname) :
        socket(address, port),
        reader(&Client::readerRoutine, this),
        printer(&Client::printerRoutine, this),
        nickname(nickname)
{}

void Client::mute() {
    muted = true;
}

void Client::unmute() {
    muted = false;
    queueCond.notify_one();
}

void Client::send(const std::string &messageText) {
    std::string time = "12:04";
    Message message{messageText, time, nickname};
    socket.write(message);
}

void Client::readerRoutine() {
    while (true) {
        if (stopped) {
            break;
        }
        Message message = socket.read();
        guard lk(queueLock);
        messageQueue.insert(message);
        queueCond.notify_one();
    }
}

void Client::printerRoutine() {
    while (true) {
        unique_lock lk(queueLock);
        queueCond.wait(lk, [this](){return !muted && !messageQueue.empty();});

        for (const Message & message : messageQueue) {
            std::cout << "<" << message.time << "> [" << message.nickname << "] " << message.text << std::endl;
        }
        messageQueue.clear();

        lk.unlock();

        if (stopped) {
            break;
        }
    }
}
