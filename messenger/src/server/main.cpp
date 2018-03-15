#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <utils/errors.h>
#include <utils/packets.h>

std::mutex incoming_message_mutex;
std::condition_variable incoming_message_cv;
std::condition_variable incoming_message_consumed_cv;
std::shared_ptr<server_packet> incoming_message;

std::mutex active_clients_mutex;
std::map<int,std::string> active_clients;

void log(std::string str) {
    std::cerr << str << "\n";
}

void disconnect(int sockfd) {
    active_clients_mutex.lock();

    active_clients.erase(sockfd);

    active_clients_mutex.unlock();

    close(sockfd);
}

void consumer_routine() {
    while (true) {
        std::unique_lock incoming_message_lock(incoming_message_mutex);

        while (!incoming_message) {
            incoming_message_cv.wait(incoming_message_lock);
        }

        active_clients_mutex.lock();

        for (auto client_info: active_clients) {
            if (client_info.second != incoming_message->sender_nickname) {
                write_packet(client_info.first, incoming_message);
            }
        }

        active_clients_mutex.unlock();

        incoming_message.reset();
        incoming_message_consumed_cv.notify_one();

        incoming_message_lock.unlock();
    }
}

void producer_routine(int sockfd) {
    bool is_finished = false;

    std::string nickname;
    
    while (!is_finished) {
        // read new packet
        std::shared_ptr<client_packet> p = read_packet<client_packet>(sockfd);

        switch (p->get_type()) {
            case LOGIN: {
                // update login
                std::string new_nickname = std::static_pointer_cast<login_client_packet>(p)->nickname;
                log("login from socket " + std::to_string(sockfd) + " -- \"" + new_nickname + "\"");
                
                if (nickname.empty() && !new_nickname.empty()) { // i.e. it's being initialized
                    nickname = new_nickname;

                    active_clients_mutex.lock();

                    active_clients[sockfd] = nickname;

                    active_clients_mutex.unlock();
                }
                break;
            }
            case MESSAGE: {
                std::string message = std::static_pointer_cast<message_client_packet>(p)->message;
                log("message from socket " + std::to_string(sockfd) + " -- \"" + message + "\"");

                // check that logged in
                if (nickname.empty()) {
                    std::cerr << NOT_LOGGED_IN_ERROR << "\n";
                } else {
                    // logged in -- make new server_packet to send to other clients
                    std::unique_lock lock(incoming_message_mutex);

                    while (incoming_message) {
                        incoming_message_consumed_cv.wait(lock);
                    }
                    
                    incoming_message.reset(new server_packet( 
                        std::time(NULL), 
                        nickname,
                        message
                    ));

                    incoming_message_cv.notify_all();
                }
                break;
            }
            case LOGOUT:
                log("logout from socket " + std::to_string(sockfd));
                is_finished = true;
                break;
            default:
                std::cerr << UNKNOWN_PACKET_ERROR << "\n";
        }
    }

    disconnect(sockfd);
}

void new_connections_listener_routine(
    int sockfd, 
    sockaddr_in cli_addr, 
    unsigned int clilen
) {
    std::vector<std::thread> producers;

    while (true) {
        /* Accept actual connection from the client */
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        check_error(newsockfd, ACCEPT_ERROR);

        log("new connection on socket " + std::to_string(newsockfd));

        // If connection is established then start communicating in new threads
        producers.push_back(std::thread(producer_routine, newsockfd));
    }
}

class socket_closer {
    int sockfd;

public:
    socket_closer(int _sockfd): sockfd(_sockfd) {}
    ~socket_closer() {
        close(sockfd);
    }
};

int main(int argc, char *argv[]) {
    int sockfd;
    uint16_t portno;
    unsigned int clilen;
    sockaddr_in serv_addr, cli_addr;
    
    if (argc < 2) {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        exit(0);
    }

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    check_error(sockfd, SOCKET_OPEN_ERROR);

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = std::stoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    check_error(
        bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)),
        SOCKET_BIND_ERROR
    );

    /* Now start listening for the clients, here process will
     * go in sleep mode and will wait for the incoming connection
    */
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // start coordinator
    std::thread consumer(consumer_routine);
    std::thread new_connections_listener(
        new_connections_listener_routine,
        sockfd,
        cli_addr,
        clilen
    );

    // just in case program finishes
    socket_closer sc(sockfd);

    new_connections_listener.join();
    consumer.join();
}