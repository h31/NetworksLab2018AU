#include "server.h"

int main(int argc, char** argv) {
	server elegram_server(12321);

	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		elegram_server.listen_new_connection();
	}

	return 0;
}