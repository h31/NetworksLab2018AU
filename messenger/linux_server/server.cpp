#include "server.h"

server::server(int port_no) {
	std::cout << "server starting..." << std::endl;
	clients = std::map<int, std::string>();

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port_no);
	server_addr.sin_family = AF_INET;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(client_sock, (sockaddr*)&server_addr, addrlen) < 0) {
        std::cout << "Failed to bind the address to our listening socket" << std::endl;
		exit(1);
	}

    if (listen(client_sock, SOMAXCONN) < 0) {
        std::cout << "Failed to listen on listening socket" << std::endl;
		exit(1);
	}

    serverptr = this;
    serv_thread = std::thread(message_sender);
    threads = std::map<int, std::thread>();
}

bool server::listen_new_connection() {
    int new_connection = accept(client_sock, (sockaddr*)&server_addr, &addrlen);
	if (new_connection == 0) {
		std::cout << "Failed to accept client connection...\n";
		return false;
	}
	else {
		std::lock_guard<std::mutex> lock(connection_mutex);
		size_t id = connections.size();
		if (unused_connections == 0) {
			std::shared_ptr<connection> new_conn(new connection(new_connection));
			connections.push_back(new_conn);
		}
		else {
			id = 0;
			while (id < connections.size() && connections[id]->is_active) {
				id++;
			}
            connections[id]->socket_fb = new_connection;
			connections[id]->is_active = true;
			unused_connections--;
		}
		std::cout << "Client connected! ID: " << id << std::endl;
        threads.insert(std::make_pair(id, std::thread(client_handler, id)));
		return true;
	}
}

bool server::process_message(int id, const std::string& time) {
	std::string message;
	if (!get_str(id, message)) return false;
	
	int autorizate_position = message.find("NAME: ");
	if (autorizate_position == 0) {
		clients.insert(std::make_pair(id, message.substr(autorizate_position + 6)));
		std::cout << "Autorizate user with id: " << id << std::endl;
		return true;
	}
	int chat_mark_position = message.find("CHAT: ");
	if (chat_mark_position == 0) {
		for (int i = 0; i < connections.size(); ++i) {
			if (!connections[i]->is_active || id == i || clients.count(id) == 0) continue;
			send_str(i, time + " " + message.substr(autorizate_position + 6));
		}
		std::cout << "Processed chat message packet from user id: " << id << std::endl;
		return true;
	}
	std::cout << "Unknown message type from user id: " << id << std::endl;
	return true;
}

void server::client_handler(int id) {
	while (true) {
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto time = "<" + std::string(std::ctime(&now));
		time = time.replace(time.size() - 1, 1, "") + ">";
		if (!serverptr->process_message(id, time)) break;
	}
	std::cout << "Lost connection to client id: " << id << std::endl;
	serverptr->disconnect_client(id);
}

void server::message_sender() {
	while (true) {
		for (int i = 0; i < serverptr->connections.size(); ++i) {
			if (serverptr->connections[i]->manager.has_pending()) {
				auto packet = serverptr->connections[i]->manager.retrieve();
				if (!serverptr->sendall(i, packet.get_buf(), packet.get_size())) {
					std::cout << "Error: message sender - Failed to send packet to ID: " << i << std::endl;
				}
				delete packet.get_buf();
			}
		}
        sleep(10);
	}
}

bool server::recvall(int id, char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
        int curr = recv(connections[id]->socket_fb, data + size, totalbytes - size, NULL);
        if (curr <= 0) return false;
		size += curr;
	}
	return true;
}

bool server::sendall(int id, char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
        int curr = send(connections[id]->socket_fb, data + size, totalbytes - size, NULL);
        if (curr <= 0) return false;
		size += curr;
	}
	return true;
}

bool server::get_int(int id, int32_t& value) {
	if (!recvall(id, (char*)&value, sizeof(int32_t))) return false;

	value = ntohl(value);
	return true;
}

void server::send_str(int id, const std::string& mess) {
	connections[id]->manager.append(packet(mess));
}

bool server::get_str(int id, std::string& mess) {
	int bufferlen;

	if (!get_int(id, bufferlen)) return false;

	char* buffer = new char[bufferlen + 1];
	buffer[bufferlen] = '\0';
								
	if (!recvall(id, buffer, bufferlen)) {
		delete[] buffer;
		return false;
	}
	mess = buffer;
	delete[] buffer;

	return true;
}

void server::disconnect_client(int id) {
	std::lock_guard<std::mutex> lock(connection_mutex);
	if (!connections[id]->is_active) return;
	connections[id]->manager.clear();						
	connections[id]->is_active = false;
    close(connections[id]->socket_fb);

	if (id == connections.size() - 1) {
		clients.erase(id);
        threads[id].join();
        threads.erase(id);
		connections.pop_back();
		id = connections.size() - 1;
		while (id >= 0 && !connections[id]->is_active) {
			clients.erase(id);
            threads[id].join();
            threads.erase(id);;
			connections.pop_back();
			id--;
			unused_connections--;
		}
	}
	else {
		unused_connections++;
	}
}

void server::close_server() {
    serv_thread.join();
}
