#include "win.h"

int read_from_socket(SOCKET sockfd, char *buffer, int len, int sz) {
    memset(buffer, 0, sz);
    int bytes_read = 0;
    while (bytes_read < len) {
    	int result = recv(sockfd, buffer + bytes_read, len - bytes_read, 0); 
    	if (result == SOCKET_ERROR) {
        	return bytes_read;
    	}
    	bytes_read += result;
    }
    return len;
}

int write_to_socket(SOCKET sockfd, char *buffer, int len) {
	int bytes_write = 0;
	while (bytes_write < len) {
		int result = send(sockfd, buffer + bytes_write, len - bytes_write, 0);
    	if (result == SOCKET_ERROR) {
        	return bytes_write;
    	}
    	bytes_write += result;
    }
    return len;
} 