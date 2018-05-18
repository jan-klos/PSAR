#pragma once

#include "Forward.hpp"

class Dtn;

class Epidemic : public Forward
{
	private:
		Dtn *dtn;
		Log &log;
		void real_broadcast(std::string &msg);
		void fake_broadcast(std::string &msg);
		void broadcast_files_list();
	public:
		Epidemic(Dtn *dtn, Log &log);
		~Epidemic();
		void start_forwarding(int *end);
		void handler_reveived_data(std::string &ip_from, char *buffer, size_t size);
		void handler_connected_peer(peer_t &peer);
		void handler_disconnected_peer(peer_t &peer);
};