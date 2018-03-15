#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <unistd.h>

#include <utils/errors.h>
#include <utils/packets.h>

enum state {
    IDLE,
    INPUT,
    QUIT
};

state current_state = IDLE;
std::mutex state_mutex;

state get_state() {
    std::lock_guard lock(state_mutex);
    return current_state;
}

void update_state(state new_state) {
    std::lock_guard lock(state_mutex);
    current_state = new_state;
}

void output_packet(std::shared_ptr<server_packet> p) {
    std::cout << "<" << std::put_time(std::localtime(&p->time_received), "%R") << ">"
              << " "
              << "[" << p->sender_nickname << "]"
              << " "
              << p->message 
              << "\n";
}

void message_reader_routine(int sockfd) {
    std::queue<std::shared_ptr<server_packet>> new_messages;

    pollfd poll_settings;
    poll_settings.fd = sockfd;
    poll_settings.events = POLLIN | POLLPRI;
    
    while (get_state() != QUIT) {
        // print queue until input process begins
        while (!new_messages.empty()) {
            std::lock_guard lock(state_mutex);
            
            if (current_state == IDLE) {
                output_packet(new_messages.front());
                new_messages.pop();
            } else {
                break;
            }
        }

        // poll whether new data is available
        const int POLL_TIMEOUT = 10;
        int poll_result = poll(&poll_settings, 1, POLL_TIMEOUT);
        check_error(poll_result, POLL_ERROR);

        if (poll_result > 0) {
            // read new packet and push to queue
            new_messages.push(read_packet<server_packet>(sockfd));
        }
    }
}

void main_loop(int sockfd) {
    while (get_state() != QUIT) {
        
        std::string line;

        std::getline(std::cin, line);

        std::shared_ptr<client_packet> p;

        if (line == ":q") {
            update_state(QUIT);

            p.reset(new logout_client_packet);
        } else if (line == ":m") {
            update_state(INPUT);

            std::getline(std::cin, line);
            
            update_state(IDLE);
            
            p.reset(new message_client_packet(line));   
        } else {
            std::cerr << "\"" << line <<  "\" -- unknown command (:q, :m expected)\n";
            continue;
        }

        /* Send message to the server */
        if (p) {
            write_packet(sockfd, p);
        }
    }
}


int main(int argc, char *argv[]) {
    int sockfd;
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
    write_packet(
        sockfd, 
        std::static_pointer_cast<client_packet>(
            std::make_shared<login_client_packet>(argv[3])
        )
    );

    // create routine to read and output incoming messages
    std::thread message_reader(message_reader_routine, sockfd);

    // start main loop which executes commands and sends packets to server
    main_loop(sockfd);
    // after it is done (got :q from standard input)

    // wait for message reader to finish
    message_reader.join();
    
    // finally close the socket
    check_error(
        close(sockfd),
        DISCONNECT_ERROR
    );

    return 0;
}