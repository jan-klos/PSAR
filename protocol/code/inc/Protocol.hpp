#pragma once

#include "Peer.hpp"

class Protocol
{
	public:
		virtual ~Protocol() {};
		//Waiting for upcoming data from a new peer
		virtual void start_listening(int *end) = 0;
		//Initialize peer's network attributes
		virtual void peer_init(peer_t *peer) = 0;
		//Create a specific protocol socket
		virtual int create_socket() = 0;
		//Properly set the sockaddr
		virtual void set_sockaddr(struct sockaddr_in *toset, std::string ip) = 0;
		//Properly set the sockopt
		virtual void set_sockopt(int socket, int seconds) = 0;
		//Send data to a specific peer
		virtual int peer_sendto(peer_t *peer, const void *buffer, size_t size) = 0;
		//Broadcast data once to everyone around
		virtual int broadcast(const void *data, size_t size) = 0;
};