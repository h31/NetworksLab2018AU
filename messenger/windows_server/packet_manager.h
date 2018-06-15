#pragma once

#include "packet.h"
#include <queue>
#include <mutex>

class packet_manager {
	std::queue<packet> packets;
	std::mutex mutex_pack;

public:
	void clear();
	bool has_pending();
	void append(const packet& pack);
	packet retrieve();
};
