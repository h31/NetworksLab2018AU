#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

const uint16_t MAX_SIZE = 32768;
uint8_t buffer[MAX_SIZE];

void convert_domain(char *name, uint8_t *dest) {
    size_t length = 0;
    uint8_t* length_index = dest;
    uint8_t* current_index = dest + 1;
    char* name_ptr = name;

    while (1) {
        char current_char = *(name_ptr++);
        if (current_char == '.' || current_char == '\0') {
            *length_index = length;
            length = 0;
            length_index = current_index++;

            if (current_char == '\0')
                break;
        } else {
            *(current_index++) = current_char;
            length++;
        }
    }

    *length_index = 0;
}

uint8_t* build_dns_request(uint8_t* dest, char* domain) {
    size_t index = 0;
    dest[index++] = 0x00; // id of transaction
    dest[index++] = 0x00;
    dest[index++] = 0x01; // query type
    dest[index++] = 0x00;
    dest[index++] = 0x00; // questions amount
    dest[index++] = 0x01;
    dest[index++] = 0x00; // answer rrs
    dest[index++] = 0x00;
    dest[index++] = 0x00; // authority rrs
    dest[index++] = 0x00;
    dest[index++] = 0x00; // additional rrs
    dest[index++] = 0x00;

    dest = dest + index;

    size_t size = 2 * strlen(domain);

    uint8_t* converted_name = malloc(size);
    convert_domain(domain, converted_name);

    memcpy(dest, converted_name, size);
    dest += size;

    index = 0;
    dest[index++] = 0x00; //type
    dest[index++] = 0x01;
    dest[index++] = 0x00; // class
    dest[index++] = 0x01;

    dest = dest + index;

    return dest;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage %s host(ip address) port hostname\n", argv[0]);
        exit(0);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    char* server_name = argv[1];
    uint16_t port = (uint16_t) atoi(argv[2]);
    
    struct sockaddr_in serv_addr;
    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_name);
    serv_addr.sin_port = htons(port);

    char* target_name = argv[3];
    uint8_t* buff_ptr = build_dns_request(buffer, target_name);

    if (sendto(sock, buffer, buff_ptr - buffer, 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        perror("ERROR with sendto");
        exit(1);
    }

    memset(buffer, 0, MAX_SIZE);
    socklen_t addr_len = sizeof(serv_addr);
    if (recvfrom(sock, buffer, MAX_SIZE, 0, (struct sockaddr*) &serv_addr, &addr_len) == -1) {
        perror("ERROR with recvfrom()");
        exit(1);
    }

    uint8_t* response = buffer;
    uint16_t questions_amount = response[5] | (response[4] << 8);
    uint16_t answers_amount   = response[7] | (response[6] << 8);

    response = response + 12;
    while (questions_amount > 0) {
        while (*response) {
            response += *response + 1;
        }

        response = response + 5;
        questions_amount--;
    }

    while (answers_amount-- > 0) {
        response += 12;
        printf("%d.%d.%d.%d\n", response[0], response[1], response[2], response[3]);
        response = response + 4;
    }

    close(sock);
    return 0;
}