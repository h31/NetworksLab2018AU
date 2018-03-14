#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

#include <netdb.h>
#include <unistd.h>

std::mutex clients_mutex;

std::vector<int> client_socket_ids;

void send_message_to_everyone(char* login, char* message, char* time, int sender_socket_id) {
    auto login_length = (uint8_t) strlen(login);
    auto message_length = (uint8_t) strlen(message);
    char buffer[518];
    buffer[0] = login_length;
    buffer[1] = message_length;
    strncpy(buffer + 2, time, 6);
    strcpy(buffer + 8, login);
    strcpy(buffer + 8 + login_length, message);
    clients_mutex.lock();
    printf("Sending message from %d\n", sender_socket_id);
    for (int socket_id : client_socket_ids) {
        ssize_t n = write(socket_id, buffer, strlen(buffer));
        if (n < 0) {
            printf("Could not write message to %d\n", socket_id);
        }
    }
    clients_mutex.unlock();
}

void register_client(int socket_id) {
    clients_mutex.lock();
    client_socket_ids.push_back(socket_id);
    printf("Client with socket %d was registered\n", socket_id);
    clients_mutex.unlock();
}

void deregister_client(int socket_id) {
    clients_mutex.lock();
    for (size_t i = 0; i < client_socket_ids.size(); i++) {
        if (client_socket_ids[i] == socket_id) {
            client_socket_ids[i] = client_socket_ids[client_socket_ids.size() - 1];
            client_socket_ids.pop_back();
            break;
        }
    }
    printf("Client with socket %d was deregistered\n", socket_id);
    clients_mutex.unlock();
}

void inner_client_handling(int socket_id) {
    /* Reading login */
    char login[256];
    memset(login, 0, sizeof(login));
    if (read(socket_id, login, 1) != 1) {
        printf("Connection with %d will be closed due to login failure\n", socket_id);
        return;
    }
    auto login_length = (uint8_t) login[0];
    login[0] = 0;
    if (read(socket_id, login, login_length) != login_length) {
        printf("Connection with %d will be closed due to login failure\n", socket_id);
        return;
    }
    /* If login is read then start communicating */
    printf("Socket id %d is %s\n", socket_id, login);
    char buffer[256];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if (read(socket_id, buffer, 1) != 1) {
            printf("Connection with %d will be closed due to read() error\n", socket_id);
            return;
        }
        auto buffer_length = (uint8_t) buffer[0];
        buffer[0] = 0;
        if (read(socket_id, buffer, buffer_length) != buffer_length) {
            printf("Connection with %d will be closed due to read() error\n", socket_id);
            return;
        }
        time_t rawtime;
        struct tm* timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char time[6];
        strftime(time, 6, "%H%M%S", timeinfo);
        printf("Received the message from %d: %s\n", socket_id, buffer);
        send_message_to_everyone(login, buffer, time, socket_id);
    }
}

void client_handling_routine(int socket_id) {
    register_client(socket_id);
    inner_client_handling(socket_id);
    deregister_client(socket_id);
    if (close(socket_id) < 0) {
        printf("ERROR closing the socket %d\n", socket_id);
    }
}

int main() {
    /* First call to socket() function */
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_socket < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    struct sockaddr_in serv_addr = {};
    memset(&serv_addr, 0, sizeof(serv_addr));
    uint16_t port_number = 5001;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    /* Now bind the host address using bind() call.*/
    if (bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    listen(listen_socket, 5);
    struct sockaddr_in cli_addr = {};
    unsigned int clilen = sizeof(cli_addr);
    while (true) {
        /* Accept actual connection from the client */
        int client_socket = accept(listen_socket, (struct sockaddr *) &cli_addr, &clilen);
        if (client_socket < 0) {
            perror("ERROR on accept");
        }
        else {
            std::thread client_thread(client_handling_routine, client_socket);
            client_thread.detach();
        }
    }
}