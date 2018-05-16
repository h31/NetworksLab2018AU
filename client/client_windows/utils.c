#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

void get_options(int argc, char** argv, char** server_addr,
                 uint16_t* server_port, char** nick_name) {
	if (argc < 4) {
		fprintf(stderr, "not all arguments are passed");
		exit(-1);
	}
	// server addr
	size_t len = strlen(argv[1]);
    *server_addr = malloc(sizeof(char) * (len + 1));
    strncpy(*server_addr, argv[1], len);
    *(*server_addr + len) = '\0';

	// server port
    *server_port = (uint16_t)(atoi(argv[2]));
    
	// nick name
	len = strlen(argv[3]);
    *nick_name = malloc(sizeof(char) * (len + 1));
    strncpy(*nick_name, argv[3], len);
    *(*nick_name + len) = '\0';
}