#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

#include "Cheat.hpp"
#include "Dtn.hpp"
#include "Epidemic.hpp"
#include "Log.hpp"
#include "Protocol.hpp"
#include "Utils.hpp"

const char* FILES_DIR = "/home/pi/files/";
const int MAX_FILES = 32;
using namespace std;
std::string files_hashes;
std::map<unsigned long, string> files_map;
unsigned long* files_hashes_arr = new unsigned long[MAX_FILES];
unsigned long* files_diff = new unsigned long[MAX_FILES];

Epidemic::Epidemic(Dtn *dtn, Log &log): dtn(dtn), log(log)
{

}

Epidemic::~Epidemic()
{

}

char* Epidemic::convert_file_to_bytes(const char *filename)  
{  
    char* filepath = new char[256];
    strcpy(filepath, FILES_DIR);
    strcat(filepath, filename);
    ifstream fl(filepath);  
    fl.seekg(0, ios::end);  
    size_t len = fl.tellg();  
    char *ret = new char[len];  
    fl.seekg(0, ios::beg);   
    fl.read(ret, len);  
    fl.close();  
    return ret;  
}  

vector<std::string> Epidemic::split_string(std::string s)
{
    std::istringstream buf(s);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> tokens(beg, end); 
    return tokens;
}

void Epidemic::create_file(std::string s)
{
	vector<std::string> msg = split_string(s);
	ofstream file;
	file.open(FILES_DIR + msg[1]); 
	int i;
	for(i = 2; i < msg.size(); i++)
	{
		file << msg[i];
		if(i != msg.size() -1)
		{
			file << " ";
		}
	}
	log_info(log, "Created file %s", msg[1]);
	file.close();

}

void Epidemic::get_files_hashes()
{
	DIR *dir;
	struct dirent *ent;
	files_hashes = "LIST ";
	int i = 0;
	if((dir = opendir(FILES_DIR)) != NULL)
	{
	  	while((ent = readdir(dir)) != NULL) 
	  	{
	  		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
	  		{
	  			continue;
	  		}
		    	files_hashes_arr[i] = std::hash<std::string>{}(ent->d_name); // les hashes de noms
		    	files_hashes.append(" ").append(to_string(files_hashes_arr[i]));
		    	files_map.insert(std::make_pair(files_hashes_arr[i], ent->d_name));
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

void Epidemic::get_files_diff(std::string neighbour_list)
{
	get_files_hashes();
	int i;
	std::fill_n(files_diff, MAX_FILES, 0);
	for(i = 0; i < MAX_FILES; i++)
	{
		if(files_hashes_arr[i] == 0)
		{
			break;
		}
		log_info(log, "File to check: %lu\n", files_hashes_arr[i]);
		if(neighbour_list.find(to_string(files_hashes_arr[i])) == std::string::npos) 
		{
			log_info(log, "File to send: %lu\n", files_hashes_arr[i]);
			files_diff[i] = files_hashes_arr[i];
		}
	}
}

void Epidemic::start_forwarding(int *end)
{
	int cpt = 1;

	get_files_hashes();

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
	broadcast_files_list();
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

void Epidemic::handler_reveived_data(std::string &ip_from, char *buffer, size_t size)
{
	(void)size;
	int i;

	if(ip_from == "0.0.0.0")
		log_warn(log, "recv from 0.0.0.0 !\n");

	if(strncmp("LIST", buffer, 4) == 0)
	{
		std::string neighbour_list(buffer, buffer + size);
		get_files_diff(neighbour_list);
		for(i = 0; i < MAX_FILES; i++) 	
		{
			if(files_diff[i] == 0)
			{
				continue;
			}
			const char* filename = files_map.find(files_diff[i])->second.c_str();
			printf("to_send: %s\n", filename);
			char* content = convert_file_to_bytes(filename);
			char* msg1 = new char(strlen(content) + strlen("FILE  \0") + strlen(filename));
			strcpy(msg1, "FILE ");
			strcat(msg1, filename);
			strcat(msg1, " ");
			strcat(msg1, content);
			strcat(msg1, "\0");
			printf("file to send: %s ", msg1);
			dtn->getProtocol()->peer_sendto(dtn->get_map().find(ip_from)->second, msg1, strlen(msg1));
		}
	}

	else if(strncmp("FILE", buffer, 4) == 0)
	{
		std::string msg(buffer, buffer + size);
		create_file(msg);

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
