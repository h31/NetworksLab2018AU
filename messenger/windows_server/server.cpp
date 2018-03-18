#include "server.h"

server::server(int port_no) {
	std::cout << "server starting..." << std::endl;
	clients = std::map<int, std::string>();

	WSAData wsa;
	WORD dll_ver = MAKEWORD(2, 1);
	if (WSAStartup(dll_ver, &wsa) != 0) {
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port_no);
	server_addr.sin_family = AF_INET;
	client_sock = socket(AF_INET, SOCK_STREAM, NULL);

	if (bind(client_sock, (SOCKADDR*)&server_addr, addrlen) == SOCKET_ERROR) {
		std::string error = "Failed to bind the address to our listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, error.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	if (listen(client_sock, SOMAXCONN) == SOCKET_ERROR) {
		std::string error = "Failed to listen on listening socket.";
		MessageBoxA(NULL, error.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	serverptr = this;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)message_sender, NULL, NULL, NULL);
}

bool server::listen_new_connection() {
	SOCKET new_connection = accept(client_sock, (SOCKADDR*)&server_addr, &addrlen);
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
			connections[id]->socket = new_connection;
			connections[id]->is_active = true;
			unused_connections--;
		}
		std::cout << "Client connected! ID: " << id << std::endl;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)client_handler, (LPVOID)(id), NULL, NULL);
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
		auto time = "<" + std::string(std::ctime(&now)) + ">";
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
		Sleep(10);
	}
}

bool server::recvall(int id, char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
		int curr = recv(connections[id]->socket, data + size, totalbytes - size, NULL);
		if (curr == SOCKET_ERROR || curr == 0) return false;
		size += curr;
	}
	return true;
}

bool server::sendall(int id, char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
		int curr = send(connections[id]->socket, data + size, totalbytes - size, NULL);
		if (curr == SOCKET_ERROR || curr == 0) return false;
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
	closesocket(connections[id]->socket);

	if (id == connections.size() - 1) {
		clients.erase(id);
		connections.pop_back();
		id = connections.size() - 1;
		while (id >= 0 && !connections[id]->is_active) {
			clients.erase(id);
			connections.pop_back();
			id--;
			unused_connections--;
		}
	}
	else {
		unused_connections++;
	}
}