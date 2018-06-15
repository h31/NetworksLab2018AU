#pragma once

#include <stdint.h>
#include <string>
#include <Windows.h>

class packet {
	int size;
	char* buf;
public:
	packet(const std::string& mess);

	int get_size();
	char* get_buf();
};
