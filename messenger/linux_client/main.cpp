#include <iostream>
#include <pthread.h>
#include <ctime>
#include <iomanip>

#include <unistd.h>

#include <string.h>
#include "../linux_commons.cpp"

using namespace std;

volatile bool completed = false;

void* message_receiver(void* arg) {
    SOCKET sock = *(SOCKET*)arg;
    while (!completed) {
        message* msg = recieve_message(sock);
        if (msg != nullptr) {
            cout << "<" << put_time(localtime(&msg->time), "%d.%m %H:%M:%S") << "> [" << msg->sender << "] " << msg->message_text << "\n";
            delete msg;
        } else {
            completed = true;
        }
    }
    return nullptr;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "usage " << string(argv[0]) << " hostname port";
        return 0;
    }

    SOCKET sockfd;
    uint16_t portno;
    sockaddr_in serv_addr;
    hostent *server;


    portno = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        handle_error("ERROR opening socket");
        return 1;
    }

    server = gethostbyname(argv[1]);

    if (server == nullptr) {
        handle_error("ERROR, no such host");
        return 0;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr, (size_t)server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "ERROR connecting";
        exit(1);
    }

    //After connection

    string nick;
    while (nick.length() == 0 || nick.length() > 100) {
        cout << "Please introduce yourself: ";
        getline(cin, nick);
    }

    pthread_t thread;
    pthread_create(&thread, nullptr, message_receiver, &sockfd);
    message msg_to_send;
    msg_to_send.sender = nick;
    while (!completed && !cin.eof()) {
        cout << ">  ";
        getline(cin, msg_to_send.message_text);
        if (msg_to_send.message_text == "\\exit") {
            completed = true;
            pthread_cancel(thread);
            break;
        }
        completed = send_message(msg_to_send, sockfd);
    }
    completed = true;
    pthread_cancel(thread);
    pthread_join(thread, nullptr);
    close(sockfd);

    return 0;
}