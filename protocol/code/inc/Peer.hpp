#pragma once

#include <thread>
#include <vector>
#include <netinet/in.h>

enum status {none, contacting, connected, suspicious, down};

typedef struct peer_s{
	int socket;
	int status;
	int signal;
	struct sockaddr_in dest_addr;
	std::string ip;
} peer_t;