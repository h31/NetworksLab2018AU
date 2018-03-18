#include "packet_manager.h"

void packet_manager::clear() {
	std::lock_guard<std::mutex> lock(mutex_pack); 
	std::queue<packet> empty;						
	std::swap(packets, empty);
}

void packet_manager::append(const packet& pack) {
	std::lock_guard<std::mutex> lock(mutex_pack);
	packets.push(pack);
}

bool packet_manager::has_pending() {
	return packets.size() > 0;
}

packet packet_manager::retrieve() {
	std::lock_guard<std::mutex> lock(mutex_pack);
	auto result = packets.front();			
	packets.pop();
	return result;
}