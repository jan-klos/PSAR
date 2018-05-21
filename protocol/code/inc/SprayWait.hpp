#pragma once

#include "Forward.hpp"

class Dtn;

class SprayWait : public Forward
{
	private:
		Dtn *dtn;
		Log &log;
		int nbcopy;
		char* send_addr;
		char* file_path;
		void real_broadcast(std::string &msg, int &nbcopy);
		void fake_broadcast(std::string &msg);
	public:
		SprayWait(Dtn *dtn, Log &log, char* send_addr, char* file_path, int nbcopy);
		~SprayWait();
		void start_forwarding(int *end);
		void handler_reveived_data(std::string &ip_from, char *buffer, size_t size);
		void handler_connected_peer(peer_t &peer);
		void handler_disconnected_peer(peer_t &peer);
};
