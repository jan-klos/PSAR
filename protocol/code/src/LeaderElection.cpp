#include "LeaderElection.hpp"
#include "Protocol.hpp"

using namespace std;

inline bool operator<(const msg_t& lhs, const msg_t& rhs)
{
	return lhs.id < rhs.id;
}

inline bool operator==(const msg_t& lhs, const msg_t& rhs)
{
	return lhs.id == rhs.id;
}

Leader::Leader(Dtn *dtn, Log &log, int my_id): dtn(dtn), log(log), my_id(my_id)
{
	msg_t me;
	me.clock = 0;
	me.id = my_id;
	me.nei.insert(my_id);
	global_known.insert(pair<int, msg_t>(my_id, me));
}

Leader::~Leader()
{

}

int Leader::current_leader()
{
	Neighbors network = get_graph();
	// string candidates;
	// for (auto n : network)
	// 	candidates.append(to_string(n) + ", ");
	// log_info(log, "Candidates are: [%s]\n", candidates.c_str());
	return *network.begin();
}

/* See LeaderElection.hpp for serialisation map */
void Leader::leader_broadcast(Global &to_send)
{
	int buff[BUFF_SIZE];
	int chunk = BUFF_SIZE/16;
	int i = 0, j = 0;

	for (int i = 0; i < BUFF_SIZE; ++i)
		buff[i] = INT_INVAL;

	for (Global::iterator it_g=to_send.begin(); it_g!=to_send.end(); ++it_g, ++i)
	{
		buff[i*chunk] 		= it_g->second.id;
		buff[i*chunk + 1] 	= it_g->second.clock;
		for (Neighbors::iterator it_n=it_g->second.nei.begin(); it_n!=it_g->second.nei.end(); ++it_n, ++j)
			buff[i*chunk + 2 + j] = *it_n;
		j = 0;
	}

	log_info(log, "SEND: %u bytes in brd\n", sizeof(buff));
	dtn->getProtocol()->broadcast(buff, BUFF_SIZE);
}

Global Leader::union_max(Global &a, Global &b)
{
	Global res;
	Global::iterator it_a = a.begin();
	Global::iterator it_b = b.begin();

	while(it_a != a.end() && it_b != b.end())
	{
		if(*it_a > *it_b)
		{
			res.insert(*it_b);
			it_b++;
		}
		else if(*it_a < *it_b)
		{
			res.insert(*it_a);
			it_a++;
		}
		else
		{
			if(it_a->second.clock > it_b->second.clock)
				res.insert(*it_a);
			else
				res.insert(*it_b);
			it_a++;
			it_b++;
		}
	}
	if(it_a == a.end())
		res.insert(it_b, b.end());
	if(it_b == b.end())
		res.insert(it_a, a.end());
	return res;
}

void Leader::start_forwarding(int *end)
{
	log_info(log, "Starting leaderElection id  = %d\n", my_id);
	while(!*end)
	{
		this_thread::sleep_for(chrono::seconds(MESH_RESCAN_DELAY_SEC));
		log_info(log, "Current leader is: %d\n", current_leader());
	}
	log_info(log, "leaderElection is done\n");
}


/* See LeaderElection.hpp for serialisation map */
void Leader::handler_reveived_data(string &ip_from, char *data, size_t size)
{
	log_info(log, "RECV: %u bytes from %s\n", size, ip_from.c_str());

	Global recv;

	int *ints = (int*)data;
	for (size_t i = 0; i < size/sizeof(int); i+=CHUNK_SIZE)
	{
		if(ints[i] != INT_INVAL)
		{
			msg_t msg;
			msg.id = ints[i];
			msg.clock = ints[i+2];
			for (size_t j = i+2; j < i+CHUNK_SIZE; ++j)
			{
				if(ints[j] == INT_INVAL)
					break;
				msg.nei.insert(ints[j]);
			}
			recv.insert(pair<int, msg_t>(ints[i], msg));
		}
	}

	recv = union_max(recv, global_known);
	global_known = recv;

	display_global(global_known);
}

void Leader::handler_connected_peer(peer_t &peer)
{
	string ip_mask = "192.168.2.";
	int id = stoi(peer.ip.substr(ip_mask.length()));
	log_notice(log, "Connected -> %d\n", id);
	global_known.find(my_id)->second.nei.insert(id);
	leader_broadcast(global_known);
}

void Leader::handler_disconnected_peer(peer_t &peer)
{
	string ip_mask = "192.168.2.";
	int id = stoi(peer.ip.substr(ip_mask.length()));
	log_warn(log, "Disconnected -> %d\n", id);
	global_known.find(my_id)->second.nei.erase(id);
	leader_broadcast(global_known);
}

void Leader::display_global(Global &a)
{
	log_info(log, "### Begin display ###\n");
	for(auto msg : a)
	{
		string neis;
		for(auto n : msg.second.nei)
			neis.append(to_string(n) + ", ");
		log_info(log, "Id: %d Clk: %d nei -> [%s]\n", 
			msg.second.id, msg.second.clock, neis.c_str());
	}
	log_info(log, "### End display ###\n");
}

Neighbors Leader::get_graph()
{
	Neighbors done;
	Neighbors res;
	int updated = 1;

	Neighbors &toAdd = global_known.find(my_id)->second.nei;

	res.insert(toAdd.begin(), toAdd.end());
	done.insert(my_id);

	while(updated)
	{
		updated = 0;
		for(auto a : res)
			if(done.find(a) == done.end())
			{
				done.insert(a);
				if(global_known.find(a) != global_known.end())
				{
					toAdd = global_known.find(a)->second.nei;
					res.insert(toAdd.begin(), toAdd.end());
				}
				updated = 1;
			}
	}

	return res;
}