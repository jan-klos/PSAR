#include <stdlib.h>
#include <thread>
#include <mutex>

#include <signal.h>

#include "Dtn.hpp"
#include "Epidemic.hpp"
#include "LeaderElection.hpp"
#include "Log.hpp"
#include "Mesh.hpp"
#include "UdpProtocol.hpp"
#include "SprayWait.hpp"

using namespace std;

//Ints to stop infinite loops of threads
int my_ints[200] = { };

Dtn::Dtn(uint network_id, int my_id, Log &log) : log(log)
{
	log_info(log, "Setting up DTN\n");
	protocol 	= new UdpProtocol(this, log);
	forward 	= new Leader(this, log, my_id);
	comm 		= new Mesh(this, log, network_id);
}

Dtn::Dtn(uint network_id, Log &log, char* send_address, char* filename, int nbcopy) : log(log)
{
	log_info(log, "Setting up DTN - Spray and Wait\n");
	protocol 	= new UdpProtocol(this, log);
	forward 	= new SprayWait(this, log, send_address, filename, nbcopy);
	comm 		= new Mesh(this, log, network_id);
}

Dtn::Dtn(uint network_id, Log &log) : log(log)
{
	log_info(log, "Setting up DTN - Epidemic\n");
	protocol 	= new UdpProtocol(this, log);
	forward 	= new Epidemic(this, log);
	comm 		= new Mesh(this, log, network_id);
}

Dtn::~Dtn()
{
	delete(forward);
	delete(protocol);
	delete(comm);
}

void Dtn::start()
{
	log_notice(log, "Starting DTN\n");

	thread thread_comm(&Communication::start_monitoring, comm, &my_ints[0]);
	thread thread_prot(&Protocol::start_listening, protocol, &my_ints[100]);
	thread thread_broad(&Forward::start_forwarding, forward, &my_ints[199]);

	signal(SIGINT, signal_handler);

	thread_broad.join();
	thread_prot.join();
	thread_comm.join();
}

int Dtn::new_peer(Station &s, string &ip)
{
	peer_t *newPeer = create_peer(ip);
	newPeer->signal = s.signal;

	if(newPeer->status == none)
	{
		log_notice(log, "New peer detected: %s\n", ip.c_str());
		protocol->peer_init(newPeer);
		newPeer->status = connected;
		forward->handler_connected_peer(*newPeer);
	}

	return 0;
}

void Dtn::lost_peer(Station &s, string &ip)
{
	peer_t *dcPeer = get_peer(ip);
	if(dcPeer)
	{
		if(s.inactive_t > LIMIT_INACTIVE_TIME_MS)
			log_warn(log, "Deleting peer %s: inactive (%u)\n", ip.c_str(), s.inactive_t);
		else if(s.signal < MIN_SIGNAL_VALUE_DBM)
			log_warn(log, "Deleting peer %s: bad signal (%d dbm)\n", ip.c_str(), s.signal);

		forward->handler_disconnected_peer(*dcPeer);

		lock_guard<mutex> guard(peers_mutex);

		if(peers.erase(ip) < 1)
			log_alert(log, "Trying to delete a non existing peer (%s)\n", ip.c_str());
		free(dcPeer);
	}
}

peer_s* Dtn::create_peer(string &ip)
{
	lock_guard<mutex> guard(peers_mutex);

	if(peers.find(ip) != peers.end())
		return peers.find(ip)->second;

	peer_t *newPeer = new peer_t;
	newPeer->ip = ip;
	newPeer->status = none;
	peers[newPeer->ip] = newPeer;

	return newPeer;
}

peer_s* Dtn::get_peer(string &ip)
{
	lock_guard<mutex> guard(peers_mutex);

	if(peers.find(ip) == peers.end())
		return NULL;
	return peers.find(ip)->second;
}

Protocol* Dtn::getProtocol()
{
	return protocol;
}

void Dtn::reveived_data(string &ip_from, char *data, size_t size)
{
	forward->handler_reveived_data(ip_from, data, size);
}

std::map<std::string, peer_s*>& Dtn::get_map()
{
	return peers;
}

void signal_handler(int sig)
{
	(void)sig;
	memset(my_ints, 1, 200*sizeof(int));
}
