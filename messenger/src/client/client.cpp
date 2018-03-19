#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <memory>
#include <time.h>

#include "socket.hpp"

void print_time()
{
	time_t t = time(0);
	struct tm * now = localtime( & t );
	std::cout << '<' << (now->tm_mon + 1) << ':' << now->tm_sec << "> ";
}

std::unique_ptr<char[]> extract_name(net::socket& sock)
{
	size_t size;

	sock.recv_all((char *)&size, sizeof(size));
	std::unique_ptr<char[]> name(new char[size + 1]);
	sock.recv_all(name.get(), size);
	name[size] = 0;

	return std::move(name);
}

std::unique_ptr<char[]> extract_msg(net::socket& sock)
{
	size_t size;

	sock.recv_all((char *)&size, sizeof(size));
	std::unique_ptr<char[]> msg(new char[size + 1]);
	sock.recv_all(msg.get(), size);
	msg[size] = 0;

	return std::move(msg);
}

void process_in(net::socket& sock)
{
	try {
		while (true) {
			auto name = extract_name(sock);
			auto msg = extract_msg(sock);

			print_time();
			std::cout << "[" << name.get() << "] " << msg.get() << std::endl;
		}
	} catch (net::network_exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void process_out(net::socket& sock, std::string name)
{
	while (true) {
		std::string msg;

		if (std::getline(std::cin, msg).eof() || msg == "exit") {
			std::cerr << "disconnect " << std::endl;
			sock.close();
			break;
		}

		size_t size = name.size();
		sock.send_all((char *)&size, sizeof(size));
		sock.send_all(name.c_str(), size);

		size = msg.size();
		sock.send_all((char *)&size, sizeof(size));
		sock.send_all(msg.c_str(), size);
	}
}

int main(int argc, char *argv[])
{
	net::socket_ops::init();

	if (argc < 4) {
		std::cerr << "usage " << argv[0] << " hostname port name\n";
		return 1;
	}

	uint16_t portno = (uint16_t) atoi(argv[2]);

	net::socket sock;
	sock.connect(argv[1], portno);
	std::cout << "connected... enter messages" << std::endl;

	std::thread th(process_in, std::ref(sock));
	try {
		process_out(sock, argv[3]);
		th.join();
	} catch (net::network_exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::cout << "end app" << std::endl;
	return 0;
}
