#pragma once

#include "Forward.hpp"
#include "Log.hpp"
#include "Peer.hpp"

#include <openssl/md5.h>

class Dtn;

struct sw_struct
{
	char* filename;
	char* send_addr;
	std::vector<std::string> used_addr;
	char hash[MD5_DIGEST_LENGTH];
	int n;
};

class SprayWait : public Forward
{
	private:
		const char* FILES_DIR = "/home/pi/files/";
		const int MAX_FILE_CONTENT = 2048;
		Dtn *dtn;
		Log &log;
		char* send_addr;
		char* filename;
		void real_broadcast(std::string &msg);
		void fake_broadcast(std::string &msg);
		void send_files(std::string ip);
	public:
		SprayWait(Dtn *dtn, Log &log, char* send_addr, char* filename, int nbcopy);
		~SprayWait();
		void start_forwarding(int *end);
		void handler_reveived_data(std::string &ip_from, char *buffer, size_t size);
		void handler_connected_peer(peer_t &peer);
		void handler_disconnected_peer(peer_t &peer);
};
