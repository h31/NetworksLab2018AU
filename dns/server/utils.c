#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>

void get_options(int argc, char** argv, uint16_t* server_port) {
    int res;
    while ((res = getopt(argc, argv, "p:")) != -1) {
        switch (res) {
            case 'p': {
                *server_port = (uint16_t)(atoi(optarg));
                break;
            }
            default: break;
        }
    }
}