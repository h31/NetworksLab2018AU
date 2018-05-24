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

u_int64_t hash_address(char* address) {
    u_int64_t res = 0;
    while (*address != 0) {
	res = res * 17 + (u_int64_t) (*(address++));
    }
    return res;
}

void run_server(SOCKET socket) {
    char request[1024];
    char response[1024];
    char domain[256];
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    dns_header request_header;
    dns_header response_header;
    while (true) {
        long received = recvfrom(socket, request, 1024, 0, (struct sockaddr *) &client, &client_len);
        if (received < 0) {
            cout << "Error: cant receive message" << endl;
            break;
        }
        char *input = request_header.parse(request);
        char *output = response + request_header.memory_size();
        response_header.clear();
        response_header.ID = request_header.ID;
        response_header.flags = (1 << 7) | (1 << 8) | (1 << 15);
        for (int i = 0; i < request_header.QDCOUNT; i++) {

            input = read_hostname(input, request, domain);
            int qtype = ntohs(*((u_int16_t*) input));
            input += 2;
            int qclass = ntohs(*((u_int16_t*) input));
            input += 2;
            if (qtype != 1 || qclass != 1) {
                response_header.flags |= 4;
                break;
            }
            output = write_hostname(domain, output);
        }
        int response_len;
        if ((response_header.flags & 4) != 0) {
            response_header.write(response);
            response_len = response_header.memory_size();;
        } else {
            response_header.QDCOUNT = request_header.QDCOUNT;
            response_header.ANCOUNT = request_header.QDCOUNT;
            
            input = request + request_header.memory_size();
            for (int i = 0; i < request_header.QDCOUNT; i++) {
            	input = read_hostname(input, request, domain);
            	input += 4;
            	output = write_hostname(domain, output);
                // write TTL
                *(output++) = 0;  
                *(output++) = 0;  
                *(output++) = 1;  
                *(output++) = 0;  
                // write RDLENGTH
                *(output++) = 0;  
                *(output++) = 4;  
	        // write "ip"
	        *((u_int64_t *) output) = htonl(hash_address(domain));
  		output += 4;
            }
            response_header.write(response);
            response_len = output - response;
        }
        sendto(socket, response, response_len, 0, (struct sockaddr *) &client, client_len);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Wrong arguments format! Expected: port" << endl;
        return 0;
    }
    uint16_t port_number = (uint16_t) atoi(argv[1]);

    SOCKET dns_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (dns_socket < 0) {
        cout << "Failed to create socket!" << endl;
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    
    if (bind(dns_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Failed to bind socket!" << endl;
        return 2;
    }

    cout << "DNS server started on port " << port_number << endl;
    run_server(dns_socket);

    if (close(dns_socket) < 0) {
        cout << "Failed to close socket!" << endl;
        return 3;
    }
    return 0;
}
