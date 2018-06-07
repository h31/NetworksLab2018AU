#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

const uint16_t MAX_SIZE = 32768;
uint8_t data_buffer[MAX_SIZE];
char message_buffer[MAX_SIZE];

void get_domain(uint8_t* client_buffer, char* dest) {
    client_buffer = client_buffer + 12;

    while (*client_buffer) {
        uint8_t *ptr = client_buffer + 1;
        while (ptr != client_buffer + *client_buffer + 1) {
            *(dest++) = *(ptr++);
        }

        *(dest++) = '.';
        client_buffer = ptr;
    }

    *(dest - 1) = '\0';
}

uint8_t* build_response(uint8_t* response, uint8_t* address) {
    response[6] = 1 >> 8;
    response[7] = 1 & 0x00ff;
    uint16_t questions_amount = response[5] | (response[4] << 8);

    response = response + 12;
    while (questions_amount > 0) {
        while (*response) {
            response += *response + 1;
        }

        response = response + 5;
        questions_amount--;
    }

    size_t index = 0;
    response[index++] = 0xc0; // name
    response[index++] = 0x0c;
    response[index++] = 0x00; // type
    response[index++] = 0x01;
    response[index++] = 0x00; // class
    response[index++] = 0x01;
    response[index++] = 0x00; // time to live
    response[index++] = 0x00;
    response[index++] = 0x00;
    response[index++] = 0xff;
    response[index++] = 0x00; // data length
    response[index++] = 0x04;

    response += index;

    memcpy(response, address, 4 * sizeof(uint8_t));
    response = response + 4;
    return response;
}

uint8_t generate_value(char* domain, uint8_t i) {
    int result = 1;
    char* ptr = domain;

    while (*ptr) {
        int character = (int) *(ptr++);
        result = (result* (character + i)) % 829;
    }

    return result % 256;
}

void get_address(char* domain, uint8_t* bytes) {
    for (int i = 0; i < 4; ++i) {
        bytes[i] = generate_value(domain, i);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(0);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        perror("ERROR opening socket");
        exit(1);
    }

    uint16_t port = (uint16_t) atoi(argv[1]);
    struct sockaddr_in addr;
    memset((char*) &addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("ERROR bind()");
        exit(1);
    }

    printf("Server started!\n");
    while (1) {
        struct sockaddr_in client_addr;

        socklen_t addr_size = sizeof(client_addr);
        if (recvfrom(sock, data_buffer, MAX_SIZE, 0, (struct sockaddr*) &client_addr, &addr_size) == -1) {
            perror("ERROR with recvfrom()");
            exit(1);
        }

        get_domain(data_buffer, message_buffer);
        uint8_t bytes[4];
        get_address(message_buffer, bytes);

        uint8_t* ptr = build_response(data_buffer, bytes);
        if (sendto(sock, data_buffer, ptr - data_buffer, 0, (struct sockaddr*) &client_addr, addr_size) == -1) {
            perror("ERROR with sendto");
            exit(1);
        }

        printf("Sent bytes: %d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    }

    close(sock);

    return 0;
}