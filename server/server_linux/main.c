#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <pthread.h>
#include <unistd.h>

void get_options(int argc, char** argv, uint16_t* server_port);
void* server(void* arg);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("pass port number for server");
        return 1;
    }
    uint16_t portno;
    get_options(argc, argv, &portno);
    pthread_t server_t;
    pthread_create(&server_t, NULL, server, &portno);
    sleep(20);
    pthread_cancel(server_t);
    pthread_join(server_t, NULL);
}
