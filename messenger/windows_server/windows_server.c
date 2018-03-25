#include <stdio.h>
#include <winsock2.h>
#include <pthread.h>
#include <time.h>
 
const unsigned short PORT = 5001;
#define CLIENTS_CAPACITY 5
#define LOGIN_CAPACITY 8
#define BUFFER_CAPACITY 4096
#define TIME_OFFSET 19
WSADATA wsa;
SOCKET server_socket;
SOCKET clients[CLIENTS_CAPACITY];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char buffer[BUFFER_CAPACITY];
char login[CLIENTS_CAPACITY][LOGIN_CAPACITY];
struct sockaddr_in server_address, client_address;

void initWSA() {
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(1);
    }
}

int write(SOCKET socket_, const char* buffer, int bytes) {
	int x = send(socket_, buffer, bytes, 0);
	return x;
}

void close(SOCKET socket_) {
	closesocket(socket_);
}

int read(SOCKET socket_, char* buffer, int bytes) {
    memset(buffer, 0, sizeof(buffer));
	int x = recv(socket_, buffer, bytes, 0);
	return x;
}

void initialize_server_socket() {
    initWSA();
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset((char*)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(1);
    }
    listen(server_socket, CLIENTS_CAPACITY);
}

int find_slot_for_client(int client_socket) {
    int index = -1;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        if (!clients[i]) {
            clients[i] = client_socket;
            index = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return index;
}

void clean_without_lock(int index) {
    memset(login[index], 0, sizeof(login[index]));
    close(clients[index]);
    clients[index] = 0;
}

void clean(int index) {
    pthread_mutex_lock(&mutex);
    clean_without_lock(index);
    pthread_mutex_unlock(&mutex);
}

void broadcast(int index, char* buffer, int bytes) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        if (!clients[i]) {
            continue;
        }
        write(clients[i], buffer, bytes);
    }
    pthread_mutex_unlock(&mutex);
}

void* client_handling_routine(void* arg) {
    SOCKET client_socket = (SOCKET)arg;
    int index = find_slot_for_client(client_socket);
    int bytes_read;
    int bytes_written;
    if (index == -1) {
        bytes_written = write(client_socket, "-", 1);
        printf("written %d\n", bytes_written);
		close(client_socket);
        return NULL;
    } else {
        bytes_written = write(client_socket, "+", 1);
        printf("written %d\n", bytes_written);
    }
   	bytes_read = read(client_socket, login[index], LOGIN_CAPACITY - 1);
    printf("read %d\n", bytes_read);
    if (bytes_read <= 0) {
        printf("ERROR on login reading");
        clean(index);
        return NULL;
    }
    char buffer[BUFFER_CAPACITY];
    int offset = strlen(login[index]) + 4 + TIME_OFFSET;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = read(client_socket, buffer + offset, BUFFER_CAPACITY - 1);
        if (bytes_read <= 0 || buffer[offset] == 1) {
            break;
        }
        time_t orig_format;
        time(&orig_format);
        memcpy(buffer, asctime(gmtime(&orig_format)), TIME_OFFSET);
        buffer[TIME_OFFSET] = '[';
        memcpy(buffer + TIME_OFFSET + 1, login[index], strlen(login[index]));
        buffer[TIME_OFFSET + 1 + strlen(login[index])] = ']';
        buffer[TIME_OFFSET + 2 + strlen(login[index])] = ':';
        buffer[TIME_OFFSET + 3 + strlen(login[index])] = ' ';
        broadcast(index, buffer, bytes_read + offset);
    }
    clean(index);
}

int main(int argc, char *argv[]) {
    initialize_server_socket();
    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < CLIENTS_CAPACITY; i++) {
        memset(login[i], 0, sizeof(login[i]));
    }
    unsigned int size_of_client_address = sizeof(client_address);
    while (1) {
        int client_socket = accept(
                server_socket,
                (struct sockaddr *) &client_address,
                &size_of_client_address
        );
        if (client_socket == INVALID_SOCKET) {
            printf("accept failed with error code : %d\n" , WSAGetLastError());
        } else {
        	printf("accepted\n");
        }
        pthread_t client_thread;
        pthread_create(
                &client_thread,
                NULL,
                client_handling_routine,
                client_socket
        );

    }
}