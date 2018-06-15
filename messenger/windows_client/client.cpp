#include "client.h"

client::client(std::string ip, int port_no, const std::string& name) {
	std::cout << "client " << name << " running..." << std::endl;
	this->name = name;
	message_queue = std::queue<std::string>();

	WSAData wsa;
	WORD dll_ver = MAKEWORD(2, 1);
	if (WSAStartup(dll_ver, &wsa) != 0) {
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	server_addr.sin_port = htons(port_no);
	server_addr.sin_family = AF_INET;

	clientptr = this;
}

bool client::connect_to_server() {
	sock = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(sock, (SOCKADDR*)&server_addr, sizeofaddr) != 0) {
		MessageBoxA(NULL, "Failed to Connect..", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)client_handler, NULL, NULL, NULL);
	return true;
}

bool client::process_message() {
	std::string message;
	if (!get_str(message)) return false;

	if (!is_input) {
		std::cout << message << std::endl;
	}
	else {
		message_queue.push(message);
	}
	return true;
}

bool client::disconnet() {
	if (closesocket(sock) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAENOTSOCK) return true;
		std::string error = "Failed to close socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(NULL, error.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

void client::client_handler() {
	bool success_autorization = true;
	if (!clientptr->send_str("NAME: " + clientptr->name)) {
		success_autorization = false;
	}
	while (true) {
		if (!success_autorization || !clientptr->process_message()) break;
	}
	std::cout << "Lost connection to the server." << std::endl;
	if (clientptr->disconnet()) {
		std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else {
		std::cout << "Socket was not able to be closed." << std::endl;
	}
}

bool client::recvall(char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
		int curr = recv(sock, data + size, totalbytes - size, NULL);
		if (curr == SOCKET_ERROR || curr == 0) return false;
		size += curr;
	}
	return true;
}

bool client::sendall(char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
		int curr = send(sock, data + size, totalbytes - size, NULL);
		if (curr == SOCKET_ERROR || curr == 0) return false;
		size += curr;
	}
	return true;
}

bool client::send_int(int32_t value) {
	value = htonl(value);
	return sendall((char*)&value, sizeof(int32_t));
}

bool client::get_int(int32_t& value) {
	if (!recvall((char*)& value, sizeof(int32_t))) return false;
	value = ntohl(value);
	return true;
}

bool client::send_str(const std::string& mess) {
	std::string message = mess;
	if (mess.find("NAME: ") == std::string::npos) {
		message = "CHAT: [" + name + "] " + mess;
	}
	int bufferlen = message.size();
	if (!send_int(bufferlen)) return false;
	bool result = sendall((char*)message.c_str(), bufferlen);
	is_input = false;
	while (!message_queue.empty()) {
		auto current = message_queue.front();
		std::cout << current << std::endl;
		message_queue.pop();
	}
	return result;
}

bool client::get_str(std::string& mess) {
	int bufferlen;
	if (!get_int(bufferlen)) return false;

	char* buffer = new char[bufferlen + 1]; 
	buffer[bufferlen] = '\0';

	if (!recvall(buffer, bufferlen)) {
		delete[] buffer;
		return false;
	}

	mess = buffer;
	delete[] buffer;
	return true;
}

void client::set_input_mode() {
	is_input = true;
}