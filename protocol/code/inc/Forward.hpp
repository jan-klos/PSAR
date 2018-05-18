#pragma once

class Forward
{
	public:
		virtual ~Forward() {};
		/* Main forward function called at the begin of the program */
		virtual void start_forwarding(int *end) = 0;
		/* Handler for each piece of data received from another peer */
		virtual	void handler_reveived_data(std::string &ip_from, char *buffer, size_t size) = 0;
		/* Handler upon connected peer */
		virtual void handler_connected_peer(peer_t &peer) = 0;
		/* Handler upon disconnected peer */
		virtual void handler_disconnected_peer(peer_t &peer) = 0;
};