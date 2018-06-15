#include "packet.h"

packet::packet(const std::string& mess) {
	size = sizeof(int32_t) + mess.size()*sizeof(char);
	buf = new char[size];
	int32_t messagesize = htonl(mess.size());
	memcpy(buf, &messagesize, sizeof(int32_t));
	memcpy(buf + sizeof(int32_t), mess.c_str(), mess.size()*sizeof(char));
}

char* packet::get_buf() {
	return buf;
}

int packet::get_size() {
	return size;
}