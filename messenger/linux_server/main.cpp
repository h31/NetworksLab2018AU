#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <unordered_set>

#include <unistd.h>

#include <string.h>
#include "../linux_commons.cpp"


using namespace std;

unordered_set<SOCKET> active_socket_vector;
pthread_mutex_t resending_mutex;

void resend_message(SOCKET sender, message& msg) {
    pthread_mutex_lock(&resending_mutex);
    for (SOCKET receiver : active_socket_vector) {
        if (receiver != sender) {
            send_message(msg, sender);
        }
    }
    pthread_mutex_unlock(&resending_mutex);
}

void* listener_for_messages(void* arg) {
    SOCKET sender = *(SOCKET*) arg;
    pthread_mutex_lock(&resending_mutex);
    active_socket_vector.insert(sender);
    pthread_mutex_unlock(&resending_mutex);
    message* msg;
    while (true) {
        msg = recieve_message(sender);
        if (msg == nullptr) {
            break;
        }
        resend_message(sender, *msg);
        delete(msg);
    }
    pthread_mutex_lock(&resending_mutex);
    active_socket_vector.erase(sender);
    pthread_mutex_unlock(&resending_mutex);
    close(sender);
    delete((SOCKET*)arg);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "usage " << string(argv[0]) << " port";
        return 0;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    uint16_t portno;
    socklen_t clilen;
    sockaddr_in serv_addr, cli_addr;

    if (sockfd < 0) {
        handle_error("ERROR opening socket");
        return 1;
    }

    /* Initialize socket structure */
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    portno = (uint16_t) atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        handle_error("ERROR on binding");
        return 1;
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    pthread_mutex_init(&resending_mutex, nullptr);

    cout << "Started\n";
    while (true) {
        pthread_t thread;
        pthread_create(&thread, nullptr, listener_for_messages, new int(accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)));
    }
}
