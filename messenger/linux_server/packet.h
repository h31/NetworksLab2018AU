#pragma once

#include <stdint.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>

class packet {
	int size;
	char* buf;
public:
	packet(const std::string& mess);

	int get_size();
	char* get_buf();
};
