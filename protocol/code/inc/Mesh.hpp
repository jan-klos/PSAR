#pragma once

#include <map>
#include <linux/nl80211.h>
#include <netlink/genl/genl.h> //struct nla_policy

#include"Communication.hpp"

typedef struct {
	int id;
	struct nl_sock* socket;
	struct nl_cb* cb;
	int result;
} Netlink;

typedef struct {
	std::string mac_addr;
	unsigned int inactive_t;
	long long unsigned rx_bytes64;
	unsigned tx_packets;
	int signal;
	int signal_avg;
} Station;

class Dtn;

class Mesh : public Communication
{
	private:
		Dtn	*dtn;
		Log	&log;
		uint network_id;
		int initNl80211(Netlink* nl);
		int getStationInfo(Netlink* nl);
		int getInterfaceInfo(Netlink* nl);
	public:
		std::map<std::string, std::string>	mapBssid;
		Mesh(Dtn *dtn, Log &log, uint network_id);
		~Mesh();
		void start_monitoring(int *end);
		void detected_peer(Station s);
		Log& get_log();
};

int getStationInfo_callback(struct nl_msg *msg, void *arg);
int getInterfaceInfo_callback(struct nl_msg *msg, void *arg);
int finish_handler(struct nl_msg *msg, void *arg);
const char *iftype_name(uint32_t iftype);