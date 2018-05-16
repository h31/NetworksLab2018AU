#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

void get_options(int argc, char** argv, char** server_addr,
                 uint16_t* server_port, char** nick_name) {
    int res;
    while ((res = getopt(argc, argv, "s:p:n:")) != -1) {
        switch (res) {
            case 's': {
                size_t len = strlen(optarg);
                *server_addr = malloc(sizeof(char) * (len + 1));
                strncpy(*server_addr, optarg, len);
                *(*server_addr + len) = '\0';
                break;
            }
            case 'p': {
                *server_port = (uint16_t)(atoi(optarg));
                break;
            }
            case 'n': {
                size_t len = strlen(optarg);
                *nick_name = malloc(sizeof(char) * (len + 1));
                strncpy(*nick_name, optarg, len);
                *(*nick_name + len) = '\0';
                break;
            }
            default: break;
        }
    }
}