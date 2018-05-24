#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>

using namespace std;

typedef int SOCKET;

class dns_header {
public:
    u_int16_t ID;
    u_int16_t flags;
    u_int16_t QDCOUNT;
    u_int16_t ANCOUNT;
    u_int16_t NSCOUNT;
    u_int16_t ARCOUNT;

    char* parse(char* buffer) {
        u_int16_t* input = (u_int16_t*) buffer;
        ID = ntohs(*(input++));
        flags = ntohs(*(input++));
        QDCOUNT = ntohs(*(input++));
        ANCOUNT = ntohs(*(input++));
        NSCOUNT = ntohs(*(input++));
        ARCOUNT = ntohs(*(input++));
        return (char*) input;
    }

    char* write(char* buffer) {
        u_int16_t* output = (u_int16_t*) buffer;
        *(output++) = htons(ID);
        *(output++) = htons(flags);
        *(output++) = htons(QDCOUNT);
        *(output++) = htons(ANCOUNT);
        *(output++) = htons(NSCOUNT);
        *(output++) = htons(ARCOUNT);
        return (char*) output;
    }

    void clear() {
        ID = 0;
        flags = 0;
        QDCOUNT = 0;
    	ANCOUNT = 0;
        NSCOUNT = 0;
        ARCOUNT = 0;
    }

    int memory_size() {
        return 2 * 6;
    }
};

char* write_hostname(char* hostname, char* dst) {
    while (*hostname != 0) {
        char* counter_char_position = dst++;
        u_int8_t cnt = 0;
        while (*hostname != '.' && *hostname != 0) {
            *(dst++) = *(hostname++);
            cnt++;
        }
        *counter_char_position = (char) cnt;
        if (*hostname == '.') {
            hostname++;
        } else {
    	    *(dst++) = 0;
	}
    }
    
    *(dst++) = 0;
    *(dst++) = 1;
    *(dst++) = 0;
    *(dst++) = 1;
    return dst;
}

char* read_hostname(char* ptr, char* message, char* dst) {
    u_int8_t name_size = (u_int8_t) *(ptr++);
    if (name_size > 63) {
	name_size ^= ((u_int8_t) 1 << 7) | ((u_int8_t) 1 << 6);
	int shift = name_size * 256 + ((u_int8_t) *(ptr++));
        read_hostname(message + shift, message, dst);
    } else {
    	memcpy(dst, ptr, name_size);
        ptr += name_size;
        dst += name_size;
        if (*ptr != 0) {
            *(dst++) = '.';
            ptr = read_hostname(ptr, message, dst);
        } else {
            *dst = 0;	
            ptr++;
	}
    }
    return ptr;
}


void parse_message(char *message) {
    char* input = message;
    dns_header header;
    input = header.parse(input);
    char tmp[256] = {};
    // skip requests
    for (int i = 0; i < header.QDCOUNT; i++) {
        input = read_hostname(input, message, tmp);
        input += 4;
    }
    // parse responses
    for (int i = 0; i < header.ANCOUNT; i++) {
        input = read_hostname(input, message, tmp);
        int type = ntohs(*((u_int16_t *) input));
        if (type != 1) {
            printf("Unsupported type in answer %d\n", type);
            continue;
        }
        cout << tmp << " has address ";
        // skip fields
        input += 10;
        for (int i = 0; i < 4; i++) {
            cout << (int) ((u_int8_t) *(input++)) << (i == 3 ? '\n' : '.');
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cout << "Wrong arguments format! Expected: server_ip port target_name" << endl;
        return 0;
    }
    u_int16_t port_number = (uint16_t) atoi(argv[2]);

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        cout << "DNS server wasn't found!" << endl;
        return 1;
    }

    struct sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    memcpy((char*) &serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port_number);

    
    // Create request-message
    char message[1024] = {};

    dns_header header;
    header.clear();
    header.ID = (u_int16_t) rand();
    header.flags = (1 << 8);
    header.QDCOUNT = 1;

    char* output = message;
    output = header.write(output);
    output = write_hostname(argv[3], output);
    int message_len = (int) (output - message);

    // Create a socket
    SOCKET dns_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (dns_socket < 0) {
        cout << "Failed to create socket!" << endl;
        return 2;
    }
    // Send request-message
    sendto(dns_socket, message, message_len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    
    // Parse response-message
    if (recv(dns_socket, message, 255, 0) < 0) {
        cout << "Error during message sending!" << endl;
        return 3;
    }
    parse_message(message);
    
    // Close socket
    if (close(dns_socket) < 0) {
        cout << "Error during socket closing!" << endl;
        return 4;
    }
    return 0;
}

