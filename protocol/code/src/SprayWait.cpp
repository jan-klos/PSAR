#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/md5.h>

#include "SprayWait.hpp"
#include "Cheat.hpp"
#include "Dtn.hpp"
#include "Log.hpp"
#include "Protocol.hpp"
#include "Utils.hpp"
#include "Dtn.hpp"

using namespace std;

const int LIMIT_NB_COPY = 32;

vector<struct sw_struct> sw;

SprayWait::SprayWait(Dtn* dtn, Log& log, char* filename,
                     char* send_addr, int nbcopy): 
                     dtn(dtn), log(log), send_addr(send_addr),
                     filename(filename)
{
	struct sw_struct s;
	s.filename = filename;
	s.send_addr = send_addr;
	s.used_addr.push_back("0.0.0.0");
	get_file_hash(FILES_DIR, filename, s.hash);
	s.n = nbcopy;
	sw.push_back(s);
}

SprayWait::~SprayWait()
{

}

void SprayWait::send_files(string ip)
{
	char* content = new char[MAX_FILE_CONTENT]();
	char* msg_to_send = new char[MAX_FILE_CONTENT]();
	if(ip == dtn->MY_IP)
	{
		return;
	}
	for(vector<struct sw_struct>::iterator it = sw.begin(); it != sw.end(); ++it) 
	{
		bool used = false;
		if(it->delivered == true || (it->n <= 1 && ip != it->send_addr))
		{
			continue;
		}
		else if(ip == it->send_addr)
		{
			it->delivered = true;
		}
	
		for(vector<string>::iterator it1 = it->used_addr.begin(); 
			it1 != it->used_addr.end(); ++it1)
		{
			if(*it1 == ip)
			{
				used = true;
				break;
			}
		}
		if(used == false)
		{
			memset(content, 0, MAX_FILE_CONTENT);
			memset(msg_to_send, 0, MAX_FILE_CONTENT);
			log_info(log, "Sending file: %s to %s\n", it->filename, ip.c_str());
			convert_file_to_bytes(FILES_DIR, it->filename, content);
			strcpy(msg_to_send, "SW_FILE ");
			strcat(msg_to_send, it->filename);
			strcat(msg_to_send, " ");
			strcat(msg_to_send, it->send_addr);
			strcat(msg_to_send, " ");
			strcat(msg_to_send, to_string((it->n)/2).c_str());
			strcat(msg_to_send, " ");
			strcat(msg_to_send, content);
			strcat(msg_to_send, "\0");
			printf("wysylam: %s %d %s\n", msg_to_send, strlen(msg_to_send), ip.c_str());
			dtn->getProtocol()->peer_sendto(dtn->get_map().find(ip)->second, msg_to_send, strlen(msg_to_send));
			printf("wyslane\n");
			it->n /= 2;
			it->used_addr.push_back(ip);
		}
	}
	//delete [] content; delete [] msg_to_send;
}


void SprayWait::start_forwarding(int *end)
{
	int cpt = 1;

	this_thread::sleep_for(chrono::seconds(5));

	log_info(log, "Broadcasting %d times with interval of %dms\n",
			BROADCAST_LIMIT, BROADCAST_DELAY_MSEC);

	while(!*end && cpt <= BROADCAST_LIMIT)
	{
		string msg = "BROADCAST_TEST" + to_string(cpt);
		real_broadcast(msg);
		cpt++;
		this_thread::sleep_for(chrono::milliseconds(BROADCAST_DELAY_MSEC));
	}

	log_info(log, "Broadcasting done\n");
}

void SprayWait::real_broadcast(string &msg)
{ 
    log_info(log, "SEND_BRD_REAL\n");
    dtn->getProtocol()->broadcast(msg.c_str(), msg.length());
}

void SprayWait::fake_broadcast(string &msg)
{
	log_info(log, "SEND_BRD_FAKE\n");
	for (auto p: dtn->get_map())
		dtn->getProtocol()->peer_sendto(p.second, msg.c_str(), msg.length());
}

void SprayWait::handler_reveived_data(std::string &ip_from, char *buffer, size_t size)
{
	(void)size;

	if(ip_from == "0.0.0.0")
	{
		log_warn(log, "recv from 0.0.0.0 !\n");
	}

	if(strncmp("SW_FILE", buffer, 7) == 0)
	{
		printf("I'm getting file\n");
		string msg(buffer, buffer + size);
		struct sw_struct s;
		create_file_sw(FILES_DIR, msg, &s);
		s.used_addr.push_back(ip_from);
		get_file_hash(FILES_DIR, filename, s.hash);
		if(strcmp(dtn->MY_IP.c_str(), s.send_addr) == 0)
		{
			s.delivered = true;
		}
		sw.push_back(s);
	}

	if(!strncmp("BROADCAST_TEST", buffer, 14))
	{
		peer_t *tmp  = dtn->get_peer(ip_from);
		if(tmp == NULL)
			log_info(log, "RECV %s from %s\n", buffer+14, ip_from.c_str());
		else
			log_info(log, "RECV %s from %s (%d dbm)\n",
					buffer+14, ip_from.c_str(), tmp->signal);
	}
	else
	{
		log_info(log, "Received -> %s\n", buffer);
		log_info(log, "From -> %s\n", ip_from.c_str());
	}
}

void SprayWait::handler_connected_peer(peer_t &peer)
{
	log_info(log, "SprayWait: %s connected\n", peer.ip.c_str());
	send_files(peer.ip);	
}
void SprayWait::handler_disconnected_peer(peer_t &peer)
{
	log_info(log, "SprayWait: %s disconnected\n", peer.ip.c_str());
}
