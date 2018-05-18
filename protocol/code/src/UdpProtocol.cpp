#include <arpa/inet.h>
#include <chrono>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <chrono>

#include "Cheat.hpp"
#include "Dtn.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include "UdpProtocol.hpp"

using namespace std;

UdpProtocol::UdpProtocol(Dtn *dtn, Log &log): dtn(dtn), log(log)
{

}

UdpProtocol::~UdpProtocol()
{

}

int UdpProtocol::create_socket()
{
	int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpSocket < 0)
		LOG_PERROR(log);
	return udpSocket;
}

void UdpProtocol::set_sockaddr(struct sockaddr_in *toset, string ip)
{
	toset->sin_family = AF_INET;
	toset->sin_port = htons(PORT);
	memset(toset->sin_zero, '\0', sizeof toset->sin_zero);

	if(ip == "INADDR_ANY")
		toset->sin_addr.s_addr = htonl(INADDR_ANY);
	else if(inet_aton(ip.c_str() , &toset->sin_addr) == 0) 
		log_alert(log, "inet_aton() failed\n");
}

void UdpProtocol::set_sockopt(int socket, int seconds)
{
	struct timeval tv;

	tv.tv_sec = seconds;
	tv.tv_usec = 0;

	if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0)
		LOG_PERROR(log);
}

//Return -1 if timer expired or error occured
//else return len with buffer set with received data
int UdpProtocol::peer_handle_recv(peer_t *peer, char *buffer, size_t size, struct sockaddr_in *from)
{
	socklen_t addrlen;
	ssize_t recvlen = recvfrom(peer->socket, buffer, size, 0, (struct sockaddr *)from, &addrlen);
	if(recvlen < 0)
	{
		if(errno != EAGAIN)
			LOG_PERROR(log);
		return -1;
	}
	return recvlen;
}

int UdpProtocol::peer_sendto(peer_t *peer, const void *buffer, size_t size)
{
	if(sendto( peer->socket, buffer, size, 0, \
			   (struct sockaddr *)&peer->dest_addr,\
			   sizeof(peer->dest_addr)) < 0)
	{
		LOG_PERROR(log);
		return -1;
	}
	return 0;
}

void UdpProtocol::start_listening(int *end)
{
	int udpSocket;
	char buffer[BUFF_SIZE];
	char hostname[BUFF_SIZE];
	struct sockaddr_in serverAddr;
	struct sockaddr_in peerAddr;
	socklen_t addrlen = sizeof(serverAddr);
	ssize_t recvlen;
	string ip;
	
	log_info(log, "Waiting for peer connection on port %d\n", PORT);

	if(gethostname(hostname, BUFF_SIZE) < 0)
		log_alert(log, "gethostname failed");

	udpSocket = create_socket();

	set_sockaddr(&serverAddr, "INADDR_ANY");

	//Set timeout to check end
	set_sockopt(udpSocket, LOOP_CHECK_DELAY_SEC);

	if(bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
		LOG_PERROR(log);

	while(!*end)
	{
		memset(buffer, '\0', BUFF_SIZE);
		recvlen = recvfrom(udpSocket, buffer, BUFF_SIZE, 0,
							(struct sockaddr *)&peerAddr, &addrlen);
		if(recvlen > 0)
		{
			string ip(inet_ntoa(peerAddr.sin_addr));
			dtn->reveived_data(ip, buffer, recvlen);
		}
		else if(errno != EAGAIN)
		{
			LOG_PERROR(log);
		}
	}

	if(close(udpSocket) < 0)
		LOG_PERROR(log);

	log_info(log, "Stop waiting for new peers, socket is now closed\n");
}

void UdpProtocol::peer_init(peer_t *peer)
{
	peer->socket = create_socket();
	set_sockaddr(&peer->dest_addr, peer->ip);
}

int UdpProtocol::broadcast(const void *data, size_t size)
{
	peer_t allPeers;
	int brd_socket, ret, one = 1;
	struct sockaddr_in broad_addr;

	brd_socket = create_socket();
	set_sockaddr(&broad_addr, BROADCAST_MESH);

	if(setsockopt(brd_socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one)) < 0)
		LOG_PERROR(log);

	allPeers.socket = brd_socket;
	allPeers.dest_addr = broad_addr;

	ret = peer_sendto(&allPeers, data, size);

	if(close(brd_socket) < 0)
		LOG_PERROR(log);

	return ret;
}