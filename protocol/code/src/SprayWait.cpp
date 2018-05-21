#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Cheat.hpp"
#include "Dtn.hpp"
#include "SprayWait.hpp"
#include "Log.hpp"
#include "Protocol.hpp"
#include "Utils.hpp"

using namespace std;

char* send_addr;
char* file_path;

SprayWait::SprayWait(Dtn* dtn, Log& log, char* send_addr,
                     char* file_path, int nbcopy): 
                     dtn(dtn), log(log), send_addr(send_addr),
                     file_path(file_path), nbcopy(nbcopy)
{

}

SprayWait::~SprayWait()
{

}


void SprayWait::start_forwarding(int *end)
{
	int cpt = 1;

	printf("%s --- %s \n", send_addr, file_path);

	this_thread::sleep_for(chrono::seconds(5));

	log_info(log, "Broadcasting %d times with interval of %dms\n",
			BROADCAST_LIMIT, BROADCAST_DELAY_MSEC);
    
	if (nbcopy > 1 && nbcopy <= LIMIT_NB_COPY){
	    while(!*end && cpt <= BROADCAST_LIMIT)
	    {   
	        nbcopy == nbcopy/2;
	        string msg = "BROADCAST_TEST" + to_string(cpt);
		    real_broadcast(msg,nbcopy);
		    cpt++;
		    this_thread::sleep_for(chrono::milliseconds(BROADCAST_DELAY_MSEC));
	    }
    }
    
    if (*end && nbcopy == 1){
        nbcopy--;
	string msg = "BROADCAST_TEST" + to_string(cpt);
        real_broadcast(msg,nbcopy);
        cpt++;
        this_thread::sleep_for(chrono::milliseconds(BROADCAST_DELAY_MSEC));
        log_info(log, "Broadcasting done\n");
    }
        
        
        
	log_info(log, "Broadcasting done\n");
}

void SprayWait::real_broadcast(string &msg, int &nbcopy)
{   
    int n = nbcopy;
    //log_info(log, "NB copy %d after broadcast\n", n)
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
		log_warn(log, "recv from 0.0.0.0 !\n");

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
}
void SprayWait::handler_disconnected_peer(peer_t &peer)
{
	log_info(log, "SprayWait: %s disconnected\n", peer.ip.c_str());
}
