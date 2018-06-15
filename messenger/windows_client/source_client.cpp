#include "client.h"

int main(int argc, char** argv) {
	std::cout << "Input: ip port_no nickname: " << std::endl;
	std::string start_string;
	getline(std::cin, start_string);
	int f_delim_pos = start_string.find(' ');
	if (f_delim_pos == std::string::npos) {
		std::cout << "Unknown client parameters";
		return 1;
	}
	int s_delim_pos = start_string.find(' ', f_delim_pos + 1);
	if (s_delim_pos <= f_delim_pos) {
		std::cout << "Unknown client parameters";
		return 1;
	}
	int port_no = atoi(start_string.substr(f_delim_pos + 1, s_delim_pos - f_delim_pos - 1).c_str());

	client me(start_string.substr(0, f_delim_pos), port_no, start_string.substr(s_delim_pos + 1));
	if (!me.connect_to_server()) {
		std::cout << "Failed to connect to server..." << std::endl;
		return 1;
	}
        std::cout << "For sending message input m: <message>" << std::endl;
	std::string input;
	while (true) {
		getline(std::cin, input);
		if (input.find("m:") == 0) {
			me.set_input_mode();
			getline(std::cin, input);
			if (!me.send_str(input)) break;
		}
		Sleep(10);
	}
}
