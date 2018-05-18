#pragma once

#include "Protocol.hpp"
#include "Peer.hpp"

class Dtn;

class UdpProtocol : public Protocol
{
	private:
		Dtn *dtn;
		Log &log;
		void peer_communicate(peer_t *peer);
		int peer_handle_recv(peer_t *peer, char *buffer, size_t size, struct sockaddr_in *from);
	public:
		UdpProtocol(Dtn *dtn, Log &log);
		~UdpProtocol();
		int create_socket();
		int peer_sendto(peer_t *peer, const void *buffer, size_t size);
		void start_listening(int *end);
		void peer_init(peer_t *peer);
		void set_sockopt(int socket, int seconds);
		void set_sockaddr(struct sockaddr_in *toset, std::string ip);
		int broadcast(const void *data, size_t size);
};