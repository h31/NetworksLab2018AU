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
    Message message{messageText, "", nickname};
    socket.write(message);
}

void Client::readerRoutine() {
    while (!stopped) {
        if (socket.interesting()) {
            try {
                Message message = socket.read();
                guard lk(queueLock);
                messageQueue.insert(message);
                queueCond.notify_one();
            } catch (const FailedToReadMessageException & ex) {
                std::cout << "connection with socket is dead" << std::endl;
                stopped = true;
                break;
            }
        } else {
            std::this_thread::yield();
        }

    }
}

void Client::printerRoutine() {
    while (!stopped) {
        unique_lock lk(queueLock);
        queueCond.wait(lk);
        if (!muted && !messageQueue.empty()) {
            for (const Message & message : messageQueue) {
                std::cout << "<" << message.time << "> [" << message.nickname << "] " << message.text << std::endl;
            }
            messageQueue.clear();
        }
        lk.unlock();
    }
}

Client::~Client() {
    stopped = true;
    if (reader.joinable()) {
        reader.join();
    }
    
    queueCond.notify_one();
    if (printer.joinable()) {
        printer.join();
    }
}

bool Client::isAlive() const {
    return !stopped;
}
