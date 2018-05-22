#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <openssl/md5.h>

#include "Cheat.hpp"
#include "Dtn.hpp"
#include "Epidemic.hpp"
#include "Log.hpp"
#include "Protocol.hpp"
#include "Utils.hpp"

using namespace std;

const char* FILES_DIR = "/home/pi/files/";
const int MAX_FILES = 32;
const int MAX_FILE_CONTENT = 128;
using namespace std;
string files_hashes;
map<string, string> files_map;
string* files_hashes_arr = new string[MAX_FILES]();
string* files_diff = new string[MAX_FILES]();

Epidemic::Epidemic(Dtn *dtn, Log &log): dtn(dtn), log(log)
{

}

Epidemic::~Epidemic()
{

}

void Epidemic::get_files_hashes()
{
	DIR *dir;
	struct dirent *ent;
	files_hashes = "LIST ";
	char hash[MD5_DIGEST_LENGTH];
	int i = 0;
	if((dir = opendir(FILES_DIR)) != NULL)
	{
	  	while((ent = readdir(dir)) != NULL) 
	  	{
	  		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
	  		{
	  			continue;
	  		}
			get_file_hash(FILES_DIR, ent->d_name, hash);
			string tmp(hash, MD5_DIGEST_LENGTH);
			files_hashes_arr[i] = tmp; 
		    files_hashes.append(" ").append(files_hashes_arr[i]);
		    files_map.insert(make_pair(files_hashes_arr[i], ent->d_name));
	    	i++;
	  	}
		closedir(dir);
	} 
	else 
	{
	  /* could not open directory */
	  perror ("");
	}
}

void Epidemic::get_files_diff(string neighbour_list)
{
	get_files_hashes();
	int i;
	fill_n(files_diff, MAX_FILES, "");
	const char* filename;
	for(i = 0; i < MAX_FILES; i++)
	{
		if(files_hashes_arr[i].empty())
		{
			break;
		}
		filename = files_map.find(files_hashes_arr[i])->second.c_str();
		log_info(log, "File to check: %s\n", filename);
		if(neighbour_list.find(files_hashes_arr[i]) == string::npos) 
		{
			log_info(log, "File to send: %s\n", filename);
			files_diff[i] = files_hashes_arr[i];
		}
	}
}

void Epidemic::start_forwarding(int *end)
{
	int cpt = 1;

	this_thread::sleep_for(chrono::seconds(2));
	/*log_info(log, "Broadcasting %d times with interval of %dms\n",
			BROADCAST_LIMIT, BROADCAST_DELAY_MSEC);

	while(!*end && cpt <= BROADCAST_LIMIT)
	{
		string msg = "BROADCAST_TEST" + to_string(cpt);
		real_broadcast(msg);
		cpt++;
		this_thread::sleep_for(chrono::milliseconds(BROADCAST_DELAY_MSEC));
	}

	log_info(log, "Broadcasting done\n");
	broadcast_files_list();*/
}

void Epidemic::real_broadcast(string &msg)
{
	log_info(log, "SEND_BRD_REAL\n");
	dtn->getProtocol()->broadcast(msg.c_str(), msg.length());
}

void Epidemic::fake_broadcast(string &msg)
{
	log_info(log, "SEND_BRD_FAKE\n");
	for (auto p: dtn->get_map())
		dtn->getProtocol()->peer_sendto(p.second, msg.c_str(), msg.length());
}

void Epidemic::broadcast_files_list()
{
	get_files_hashes();
	real_broadcast(files_hashes);
}

void Epidemic::handler_reveived_data(string &ip_from, char *buffer, size_t size)
{
	(void)size;
	int i;

	if(ip_from == "0.0.0.0")
		log_warn(log, "recv from 0.0.0.0 !\n");

	if(ip_from == dtn->MY_IP)
	{
		log_info(log, "Message from myself: %s\n", ip_from.c_str());
		return;
	}

	if(strncmp("LIST", buffer, 4) == 0)
	{
		string neighbour_list(buffer, buffer + size);
		get_files_diff(neighbour_list);
		char* content = new char[MAX_FILE_CONTENT]();
		char* msg_to_send = new char[MAX_FILE_CONTENT]();
		for(i = 0; i < MAX_FILES; i++) 	
		{
			if(files_diff[i].empty())
			{
				continue;
			}
			memset(content, 0, MAX_FILE_CONTENT);
			memset(msg_to_send, 0, MAX_FILE_CONTENT);
			const char* filename = files_map.find(files_diff[i])->second.c_str();
			log_info(log, "Sending file: %s to %s\n", filename, ip_from.c_str());
			convert_file_to_bytes(FILES_DIR, filename, content);
			strcpy(msg_to_send, "FILE ");
			strcat(msg_to_send, filename);
			strcat(msg_to_send, " ");
			strcat(msg_to_send, content);
			strcat(msg_to_send, "\0");
			dtn->getProtocol()->peer_sendto(dtn->get_map().find(ip_from)->second, msg_to_send, strlen(msg_to_send));
		}
		//delete [] content; delete [] msg_to_send;
	}

	else if(strncmp("FILE", buffer, 4) == 0)
	{
		string msg(buffer, buffer + size);
		create_file(FILES_DIR, msg);

		broadcast_files_list();
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

void Epidemic::handler_connected_peer(peer_t &peer)
{
	log_info(log, "Epidemic: %s connected\n", peer.ip.c_str());
	broadcast_files_list();
}
void Epidemic::handler_disconnected_peer(peer_t &peer)
{
	log_info(log, "Epidemic: %s disconnected\n", peer.ip.c_str());
}
 