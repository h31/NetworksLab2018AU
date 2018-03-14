#include <cstdio>
#include <cstdlib>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>
#include <cstring>

#include <errors.h>
#include <packets.h>

int client_delta;
pthread_mutex_t client_changes_mutex;
pthread_condition_t client_changes_condition;

server_packet new_packet;
pthread_mutex_t new_packet_mutex;
pthread_condition_t new_packet_condition;

pthread_condition_t ready_to_read_condition;

pthread_barrier_t ready_clients_barrier;

void *client_coordinator_routine(void *arg) {
    client_changes_mutex = PTHREAD_MUTEX_INITIALIZER;
    client_changes_condition = PTHREAD_COND_INITIALIZER;

    new_packet_mutex = PTHREAD_MUTEX_INITIALIZER;
    new_packet_condition = PTHREAD_COND_INITIALIZER;

    ready_to_read_condition = PTHREAD_COND_INITIALIZER;

    int client_count = 0;

    while (true) {
        // process client count changes
        pthread_mutex_lock(&client_changes_mutex);
        client_count += client_delta;
        
        // update barrier to hold new client count + coordinator
        pthread_barrier_init(&ready_clients_barrier, client_count + 1);

        // wake up changed clients
        pthread_cond_broadcast(&client_changes_condition);

        pthread_mutex_unlock(&client_changes_mutex);

        // allow one client to introduce its message to everyone
        pthread_cond_signal(&ready_to_read_condition);

        // wait for new packet a to be sent to each active client
        pthread_barrier_wait(&ready_clients_barrier);
    }
}

struct client_shared_data {
    const int sockfd;
    volatile std::string nickname;
    volatile bool is_finished;
};

void *client_reader_routine(void *arg) {
    client_shared_data *data = (client_shared_data *) arg;

    // we are a new client
    pthread_mutex_lock(&client_changes_mutex);
    
    ++client_delta;
    pthread_cond_wait(&client_changes_condition, &client_changes_mutex);

    pthread_mutex_unlock(&client_changes_mutex);

    // once coordinator acknowledged our presence we start iteration
    while (!data->is_finished) {
        // read packet
        client_packet p = read_client_packet(data->sockfd);

        switch (p.type) {
            LOGIN:
                // update login
                data->nickname = std::static_cast<login_client_packet>(p).nickname;
                break;
            MESSAGE:
                // check logged in
                if (data->nickname.empty()) {
                    std::cerr << NOT_LOGGED_IN_ERROR << "\n";
                } else {
                    // logged in -- make new server_packet to send to other clients
                    pthread_mutex_lock(&new_packet_mutex);
    
                    // wait to be given word and then send packet
                    pthread_cond_wait(&new_packet_condition, &new_packet_mutex);
                    new_packet = { 
                        std::time(NULL), 
                        data->nickname,  
                        std::static_cast<login_client_packet>(p).message
                    };

                    pthread_mutex_unlock(&new_packet_mutex);
                }
                break;
            LOGOUT:
                // finish iteration
                data->is_finished = true;
                break;
            default:
                std::cerr << UNKNOWN_PACKET_ERROR << "\n";
        }
    }
}

void *client_writer_routine(void *arg) {
    client_shared_data *data = (client_shared_data *) arg;
    
    while (!data->is_finished) {
        pthread_mutex_lock(&new_packet_mutex);

        // wait for new packet to send
        pthread_cond_wait(&new_packet_condition, &new_packet_mutex);

        // check that we don't send our packet to ourselves
        if (new_packet.nickname != data->nickname) {
            write_packet(data->sockfd, new_packet);
        }

        pthread_mutex_unlock(&new_packet_mutex);

        // wait for other clients to send the packet
        pthread_barrier_wait(&ready_clients_barrier);
    }

    // finalizing procedure
    pthread_mutex_lock(&client_changes_mutex);
    
    // clear shared data
    delete data;

    // tell coordinator we are retiring and wait until it allows us to leave
    --client_delta;
    pthread_cond_wait(&client_changes_condition, &client_changes_mutex);

    pthread_mutex_unlock(&client_changes_mutex);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    uint16_t portno;
    unsigned int clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    ssize_t n;

    if (argc < 2) {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        exit(0);
    }

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    check_error(sockfd, SOCKET_OPEN_ERROR);

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = argv[1];

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
    thread_t coordinator;

    pthread_create(
        &coordinator, 
        NULL, 
        client_coordinator_routine,
        NULL
    );

    while (true) {
        /* Accept actual connection from the client */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        check_error(newsockfd, ACCEPT_ERROR);

        // If connection is established then start communicating in new threads
        client_shared_data *shared_data = new client_shared_data { newsockfd, "", false };

        pthread_create(
            new pthread_t,
            NULL,
            client_reader_routine,
            shared_data
        );

        pthread_create(
            new pthread_t,
            NULL,
            client_writer_routine,
            shared_data
        );
    }

    return 0;
}
