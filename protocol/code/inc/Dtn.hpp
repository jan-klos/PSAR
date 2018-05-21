#pragma once

#include <map>
#include <mutex>

#include "Peer.hpp"
#include "Utils.hpp"
#include "Log.hpp"
#include "Mesh.hpp"

class Communication;
class Forward;
class Protocol;

void signal_handler(int sig);

class Dtn 
{
	private:
		Log			 	&log;
		Communication 	*comm;
		Protocol 		*protocol;
		Forward 		*forward;
		std::map<std::string, peer_s*> peers;
		std::mutex peers_mutex;

	public:
		std::string MY_IP = "192.168.2.2"; // TODO maintenant en dur
		const char* FILES_DIR = "/home/pi/files/"; // TODO maintenant en dur
		const int MAX_FILES = 32;
		const int MAX_FILE_CONTENT = 2048;
		Dtn(uint network_id, int my_id, Log &log);
		Dtn(uint network_id, Log &log);
		Dtn(uint network_id, Log &log, char* send_address, char* filename, int nbcopy);
		~Dtn();
		void start();
		int new_peer(Station &s, std::string &ip);
		void lost_peer(Station &s, std::string &ip);
		peer_s* create_peer(std::string &ip);
		peer_s* get_peer(std::string &ip);
		void reveived_data(std::string &ip_from, char *data, size_t size);
		Protocol* getProtocol();
		std::map<std::string, peer_s*>& get_map();
};