#include "client.h"

client::client(std::string ip, int port_no, const std::string& name) {
    std::cout << "client " << name << " running..." << std::endl;
    this->name = name;
    message_queue = std::queue<std::string>();

    hostent * server = gethostbyname(ip.c_str());
    if (!server) {
        std::cout << "ERROR, no such host\n";
        exit(0);
    }

    sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    server_addr.sin_port = htons(port_no);
    server_addr.sin_family = AF_INET;

	clientptr = this;
}

bool client::connect_to_server() {
    socket_fb = socket(AF_INET, SOCK_STREAM, NULL);
    std::cout << socket_fb;
    if (connect(socket_fb, (sockaddr*)&server_addr, sizeofaddr) < 0) {
        std::cout << "Failed to Connect.." << std::endl;
		return false;
    }
    client_thread = std::thread(client_handler);
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
    if (close(socket_fb) <= 0) {
        std::cout << "Failed to close socket" << std::endl;
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
    clientptr->client_thread.join();
}

bool client::recvall(char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
        int curr = recv(socket_fb, data + size, totalbytes - size, NULL);
        if (curr <= 0) return false;
		size += curr;
	}
	return true;
}

bool client::sendall(char* data, int totalbytes) {
	int size = 0;
	while (size < totalbytes) {
        int curr = send(socket_fb, data + size, totalbytes - size, NULL);
        if (curr <= 0) return false;
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
