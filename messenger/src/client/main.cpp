#include <cstdio>
#include <cstdlib>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>
#include <cstdio>
#include <iostream>

#include <utils/errors.h>
#include <packets.h>

const uint BUFFER_SIZE = 256;

enum state {
    IDLE,
    INPUT,
    QUIT
};

volatile state current_state;
pthread_mutex_t state_mutex;

void output_packet(packet p) {
    std::cout << "<" << std::put_time(std::localtime(p.time_received), ""R(%R)"") << ">"
              << " "
              << "[" << p.sender_nickname << "]"
              << " "
              << p.message 
              << "\n";
}

void* message_reader_routine(void* arg) {
    int sockfd = *((int *) arg);
    std::queue<server_packet> new_messages;
    
    while (current_state != QUIT) {
        // print queue until input process begins
        while (!new_messages.empty()) {
            pthread_mutex_lock(&state_mutex);
            
            if (current_state == IDLE) {
                output_packet(new_messages.front());
                new_messages.pop();
                pthread_mutex_unlock(&state_mutex);
            } else {
                pthread_mutex_unlock(&state_mutex);
                break;
            }
        }

        // read new packet and push to queue
        server_packet incoming_packet = std::static_cast<server_packet>(read_packet(sockfd));
        new_messages.push(incoming_packet);
    }

    return NULL;
}

void main_loop(int sockfd) {
    while (current_state != QUIT) {
        std::string line;

        std::getline(std::cin, line);

        pthread_mutex_lock(&state_mutex);

        client_packet p;

        if (line == ":q") {
            current_state = QUIT;

            p = logout_client_packet {};
        } else if (line == ":m") {
            current_state = INPUT;

            pthread_mutex_unlock(&state_mutex);

            std::getline(std::cin, line);

            pthread_mutex_lock(&state_mutex);

            current_state = IDLE;

            pthread_mutex_unlock(&state_mutex);

            p = message_client_packet {line};
            
            pthread_mutex_lock(&state_mutex);
        } else {
            std::cerr << "\"" << line <<  "\" -- unknown command (:q, :m expected)\n";
            continue;
        }

        pthread_mutex_unlock(&state_mutex);

        /* Send message to the server */
        write_packet(sockfd, &p);
    }
}


int main(int argc, char *argv[]) {
    int sockfd, n;
    uint16_t portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4) {
        fprintf(stderr, "usage: %s hostname port nickname\n", argv[0]);
        exit(0);
    }

    if (!strlen(argv[3])) {
        fprintf(stderr, "nickname should not be empty\n");
        exit(0);
    }

    portno = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    check_error(sockfd, SOCKET_OPEN_ERROR);

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    check_error(
        connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)),
        CONNECT_ERROR
    );

    // send nickname to server
    write_client_packet(sockfd, login_client_packet {argv[3]} );

    // initialize threads
    state_mutex = PTHREAD_MUTEX_INITIALIZER;
    current_state = IDLE;

    pthread_t message_reader;
    
    // create routine to read and output incoming messages
    pthread_create(
        &message_reader, 
        NULL, 
        message_reader_routine,
        &sockfd
    );

    // start main loop which executes commands and sends packets to server
    main_loop(sockfd);
    // after it is done (:q got from standard input)

    // wait for message reader to finish
    pthread_join(message_reader, NULL);

    // finally close the socket
    check_error(
        close(sockfd),
        DISCONNECT_ERROR
    );

    return 0;
}