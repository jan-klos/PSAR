#pragma once

#include <map>
#include <set>
#include <string>

#include "Dtn.hpp"
#include "Log.hpp"
#include "Peer.hpp"
#include "Forward.hpp"

/****************   SERIALIZATION   ****************
 *
 *	|     CHUNK_SIZE     |    CHUNK_SIZE      | ...
 *	| <-- struct msg --> | <-- struct msg --> | ...
 *	|id|clk|nei1|nei2|...|id|clk|nei1|nei2|...| ...
 */

typedef std::set<int> Neighbors;

typedef struct msg{
	int id;
	int clock;
	Neighbors nei;
} msg_t;

typedef std::map<int, msg_t> Global;

class Leader : public Forward
{
	private:
		Dtn	*dtn;
		Log	&log;
		int my_id;
		Global global_known;
		int current_leader();
		Neighbors get_graph();
		// merge a and b keeping highest clocks
		Global union_max(Global &a, Global &b);
		void display_global(Global &a);
		void leader_broadcast(Global &to_send);
	public:
		Leader(Dtn *dtn, Log &log, int my_id);
		~Leader();
		void start_forwarding(int *end);
		void handler_reveived_data(std::string &ip_from, char *data, size_t size);
		void handler_connected_peer(peer_t &peer);
		void handler_disconnected_peer(peer_t &peer);
};