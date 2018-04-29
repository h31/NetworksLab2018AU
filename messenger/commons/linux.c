#include "linux.h"

#include <unistd.h>
#include <string.h>

int read_from_socket(int sockfd, char *buffer, int len, int sz) {
    bzero(buffer, sz);
    int bytes_read = 0;
    while (bytes_read < len) {
        int n = read(sockfd, buffer + bytes_read, len - bytes_read); 
        if (n < 0) return bytes_read;
        bytes_read += n;
    }
    return len;
}

int write_to_socket(int sockfd, char *buffer, int len) {
    int bytes_write = 0;
    while (bytes_write < len) {
        int n = write(sockfd, buffer + bytes_write, len - bytes_write); 
        if (n < 0) return bytes_write;
        bytes_write += n;
    }
    return len;
}