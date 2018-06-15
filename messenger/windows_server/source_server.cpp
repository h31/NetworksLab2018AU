#include "server.h"

int main(int argc, char** argv) {
        std::cout << "Input: port_no: " << std::endl;
        int port_no = 0;
        std::cin >> port_no;
        server elegram_server(port_no);

	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		elegram_server.listen_new_connection();
	}

	return 0;
}
