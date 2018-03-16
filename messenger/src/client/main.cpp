#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include <network.h>
#include <utils/errors.h>
#include <utils/packets.h>

enum state {
    STATE_IDLE,
    STATE_INPUT,
    STATE_QUIT
};

state current_state = STATE_IDLE;
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

void message_reader_routine(SOCKET sockfd) {
    std::queue<std::shared_ptr<server_packet>> new_messages;

    while (get_state() != STATE_QUIT) {
        // print queue until input process begins
        while (!new_messages.empty()) {
            std::lock_guard lock(state_mutex);
            
            if (current_state == STATE_IDLE) {
                output_packet(new_messages.front());
                new_messages.pop();
            } else {
                break;
            }
        }

        if (ready_to_read(sockfd)) {
            // read new packet and push to queue
            try {
                new_messages.push(read_packet<server_packet>(sockfd));
            } catch (std::string message) {
                std::cerr << message << "\n";
                update_state(STATE_QUIT);
            }
        }
    }
}


void main_loop(SOCKET sockfd) {
    while (get_state() != STATE_QUIT) {
        
        std::string line;

        std::getline(std::cin, line);

        std::shared_ptr<client_packet> p;

        if (line == ":q") {
            update_state(STATE_QUIT);

            p.reset(new logout_client_packet);
        } else if (line == ":m") {
            update_state(STATE_INPUT);

            std::getline(std::cin, line);
            
            update_state(STATE_IDLE);
            
            p.reset(new message_client_packet(line));   
        } else {
            std::cerr << "\"" << line <<  "\" -- unknown command (:q, :m expected)\n";
            continue;
        }

        /* Send message to the server */
        if (p) {
            try {
                write_packet(sockfd, p);
            } catch (std::string message) {
                std::cerr << message << "\n";
                update_state(STATE_QUIT);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    SOCKET sockfd;
    ADDRINFOA *server_info = NULL;
    
    if (argc < 4) {
        fprintf(stderr, "usage: %s hostname port nickname\n", argv[0]);
        exit(0);
    }

    if (!strlen(argv[3])) {
        fprintf(stderr, "nickname should not be empty\n");
        exit(0);
    }

    // retrieve information about server
    ADDRINFOA hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int n = getaddrinfo(argv[1], argv[2], &hints, &server_info);
    if (n != 0) {
        fprintf(stderr, "%s: %s\n", gai_strerror(n), ADDRINFO_ERROR.c_str());
    }

    /* Create a socket point */
    sockfd = socket(server_info->ai_family, server_info->ai_socktype, 0);
    check_error(sockfd, SOCKET_OPEN_ERROR);

    /* Now connect to the server */
    check_error(
        connect(sockfd, server_info->ai_addr, server_info->ai_addrlen),
        CONNECT_ERROR
    );

    // send nickname to server
    try {
        write_packet(
            sockfd, 
            std::static_pointer_cast<client_packet>(
                std::make_shared<login_client_packet>(argv[3])
            )
        );
    } catch (std::string message) {
        std::cerr << message << "\n";
        return 0;
    }

    // create routine to read and output incoming messages
    std::thread message_reader(message_reader_routine, sockfd);

    // start main loop which executes commands and sends packets to server
    main_loop(sockfd);
    // after it is done (got :q from standard input)

    // wait for message reader to finish
    message_reader.join();
}