#include <list>
#include <thread>
#include <mutex>
#include <iostream>
#include <memory>

#include "socket.hpp"

#define DEBUG_TOKEN "[SERVER] "

struct client {
	net::socket sock;
	std::thread th;

	explicit client(net::socket&& sock)
		: sock(std::move(sock))
	{}

	client(client&&) = default;

	bool operator==(const client& rhs) const
	{
		return sock == rhs.sock;
	}

	bool operator!=(const client& rhs) const
	{
		return !(*this == rhs);
	}
};

struct server {
	std::list<client> clients;
	std::mutex mtx;
	net::socket listen;

	server() = default;
};

template <class Foo>
static void process_broadcast(server& server, client& cur, Foo foo)
{
		for (client& client: server.clients)
			if (client != cur)
				foo(client);
}

static void process_round(server& server, client& cur)
{
	uint32_t name_sz;
	uint32_t msg_sz;

	cur.sock.recv_all((char *)&name_sz, sizeof(name_sz));
	name_sz =  htonl(name_sz);
	std::unique_ptr<char[]> name_frame(new char[name_sz + sizeof(name_sz) + 1]);
	((uint32_t*)name_frame.get())[0] = htonl(name_sz);
	cur.sock.recv_all(name_frame.get() + sizeof(name_sz), name_sz);
	name_frame[name_sz + sizeof(name_sz)] = 0;

	cur.sock.recv_all((char *)&msg_sz, sizeof(msg_sz));
	msg_sz = htonl(msg_sz);
	std::unique_ptr<char[]> msg_frame(new char[msg_sz + sizeof(msg_sz) + 1]);
	cur.sock.recv_all(msg_frame.get() + sizeof(msg_sz), msg_sz);
	((uint32_t*)msg_frame.get())[0] = htonl(msg_sz);

	char *const msg = msg_frame.get() + sizeof(msg_sz);
	char *const name = name_frame.get() + sizeof(name_sz);
	msg[msg_sz] = 0;
	name[name_sz] = 0;
	std::cout << DEBUG_TOKEN "msg: " << name << ", " << msg << std::endl;

	try {
		std::lock_guard<std::mutex> guard(server.mtx);
		process_broadcast(server, cur, [=, &name_frame, &msg_frame](client& client) {
			client.sock.send_all(name_frame.get(), name_sz + sizeof(name_sz));
			client.sock.send_all(msg_frame.get(), msg_sz + sizeof(msg_sz));
		});
	} catch (net::network_exception& e) {
		// ignore broken connection
	}
}

static void process(server& server, std::list<client>::iterator it)
{
	client& cur = *it;

	try {
		std::cout << DEBUG_TOKEN "client accepted" << std::endl;
		while (true)
			process_round(server, cur);

	} catch (net::network_exception& e) {
		std::lock_guard<std::mutex> guard(server.mtx);
		std::cout << "client disconnected [" << e.what() << "]" << std::endl;
		cur.sock.close();
		server.clients.erase(it);
	}
}


static void clear_server(server& server)
{
	for (client& client: server.clients)
		client.sock.close();
	for (client& client: server.clients)
		client.th.join();
}

int main(int argc, char *argv[])
{
	net::socket_ops::init();

	if (argc < 2) {
		std::cerr << "usage " << argv[0] << " port\n";
		return 1;
	}

	uint16_t portno = (uint16_t) atoi(argv[1]);
	server server;

	std::cout << DEBUG_TOKEN "start listening" << std::endl;

	try {
		server.listen.listen(portno);
		std::cout << DEBUG_TOKEN "start accepting" << std::endl;

		while (true) {
			client cl(server.listen.accept());

			std::lock_guard<std::mutex> guard(server.mtx);
			server.clients.push_front(std::move(cl));
			server.clients.back().th = std::thread(
				process,
				std::ref(server),
				server.clients.begin()
			);
			server.clients.back().th.detach();
		}
	} catch (net::network_exception& e) {
		clear_server(server);
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
