#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void get_options(int argc, char** argv, uint16_t* server_port) {
	if (argc < 2) {
		fprintf(stderr, "didn't pass server port through command line args");
		exit(-1);
	}
	*server_port = (uint16_t)(atoi(argv[1]));
}
