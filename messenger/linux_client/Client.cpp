#include <iostream>
#include "Client.h"

#define VERBOSE_COUT if (false) std::cout
//#define VERBOSE_COUT std::cout

Client::Client(const std::string &address, uint16_t port, const std::string &nickname) :
        socket(address, port),
        muted(false),
        stopped(false),
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
    VERBOSE_COUT << "client: reader thread started" << std::endl;
    while (!stopped) {
        VERBOSE_COUT << "client: checking interest" << std::endl;
        if (socket.interesting()) {
            try {
                VERBOSE_COUT << "client: interesting: waiting for message!" << std::endl;
                Message message = socket.read();
                VERBOSE_COUT << "client: got message!" << std::endl;
                guard lk(queueLock);
                messageQueue.insert(message);
                queueCond.notify_one();
            } catch (const SocketException & ex) {
                std::cout << "connection with socket is dead" << std::endl;
                stopped = true;
                break;
            }
        } else {
            std::this_thread::yield();
        }
    }
    VERBOSE_COUT << "client: reader thread ended" << std::endl;
}

void Client::printerRoutine() {
    VERBOSE_COUT << "client: printer thread started" << std::endl;
    while (!stopped) {
        VERBOSE_COUT << "client: printer cycle!" << std::endl;
        unique_lock lk(queueLock);
        queueCond.wait(lk);
        VERBOSE_COUT << "client: try print: " << muted << " " << messageQueue.empty() << std::endl;
        if (!muted && !messageQueue.empty()) {
            VERBOSE_COUT << "client: got messages to print!" << std::endl;
            for (const Message & message : messageQueue) {
                std::cout << "<" << message.time << "> [" << message.nickname << "] " << message.text << std::endl;
            }
            messageQueue.clear();
        }
        lk.unlock();
    }
    VERBOSE_COUT << "client: printer thread ended" << std::endl;
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
