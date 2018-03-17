#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>

#include <string.h>


#define MAX_LOGIN_LEN 256
#define MAX_MSG_LEN 256

volatile int print_pause = 0;

int read_from_socket(int sockfd, char *buffer, int len, int sz) {
    bzero(buffer, sz);
    int n = read(sockfd, buffer, len); 
    if (n < 0) perror("ERROR reading from socket");
    return n;
}

void *receiving_messages(void *arg) {
    int sockfd = *((int *)arg);

    char header[4], login[MAX_LOGIN_LEN], msg[MAX_MSG_LEN];
    while (read_from_socket(sockfd, header, 4, 4) > 0) {
        uint8_t hours, mins, login_len, msg_len;
        memcpy(&hours, header, 1);
        memcpy(&mins, header + 1, 1);
        memcpy(&login_len, header + 2, 1);
        memcpy(&msg_len, header + 3, 1);
        if (read_from_socket(sockfd, login, login_len, MAX_LOGIN_LEN) <= 0) return NULL;
        if (read_from_socket(sockfd, msg, msg_len, MAX_MSG_LEN) <= 0) return NULL;
        while (print_pause) {}
        printf("<%02d:%02d> [%s] %s", hours, mins, login, msg);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port login\n", argv[0]);
        exit(0);
    }

    uint16_t portno = (uint16_t) atoi(argv[2]);
    char login[MAX_LOGIN_LEN];
    strcpy(login, argv[3]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    if (write(sockfd, login, strlen(login)) != (long) strlen(login)) {
        perror("ERROR on writing to socket");
        exit(1);
    }
    pthread_t client;
    pthread_create(&client, NULL, receiving_messages, (void *) &sockfd);

    char buffer[MAX_MSG_LEN];
    while (1) {
        bzero(buffer, MAX_MSG_LEN);
        fgets(buffer, MAX_MSG_LEN - 1, stdin);  
        if (strcmp(buffer, "m\n") == 0) {
            bzero(buffer, MAX_MSG_LEN);
            print_pause = 1;
            
            time_t now = time(NULL);
            struct tm *now_tm = localtime(&now);
            int hours = now_tm->tm_hour;
            int mins = now_tm->tm_min;
            printf("<%02d:%02d> [%s] ", hours, mins, login);
            fgets(buffer, MAX_MSG_LEN - 1, stdin);    
            print_pause = 0;
            if (write(sockfd, buffer, strlen(buffer)) != (long) strlen(buffer)) {
                perror("ERROR on writing to socket");
                exit(1);
            }
        } 
    }

    pthread_join(client, NULL);
    close(sockfd);
    
    return 0;
}