#include <string>
#include <vector>
#include <chrono>

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <net/if.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>  
#include <linux/nl80211.h>

#include"Cheat.hpp"
#include"Dtn.hpp"
#include"Mesh.hpp"
#include"Log.hpp"
#include"Utils.hpp"

#define _XOPEN_SOURCE 700

using namespace std;

static struct nla_policy station_policy[NL80211_STA_INFO_MAX + 1];

static const char *ifmodes[NL80211_IFTYPE_MAX + 1] = {
	"unspecified",
	"IBSS",
	"managed",
	"AP",
	"AP/VLAN",
	"WDS",
	"monitor",
	"mesh point",
	"P2P-client",
	"P2P-GO",
	"P2P-device",
	"outside context of a BSS",
	"NAN",
};

Mesh::Mesh(Dtn *dtn, Log &log, uint network_id): dtn(dtn), log(log), network_id(network_id)
{
	mapBssid.insert(pair<string,string>(MAC_ADDR_RPI1, IP_RPI1_MESH));
	mapBssid.insert(pair<string,string>(MAC_ADDR_RPI2, IP_RPI2_MESH));
	mapBssid.insert(pair<string,string>(MAC_ADDR_RPI3, IP_RPI3_MESH));
	mapBssid.insert(pair<string,string>(BROADCAST_STR, BROADCAST_MESH));
}

Mesh::~Mesh()
{

}

int Mesh::initNl80211(Netlink* nl)
{
	nl->socket = nl_socket_alloc();
	if (!nl->socket)
	{ 
		log_alert(log, "Failed to allocate netlink socket.\n");
		return -ENOMEM;
	}  

	nl_socket_set_buffer_size(nl->socket, 8192, 8192);

	if (genl_connect(nl->socket))
	{ 
		log_alert(log, "Failed to connect to netlink socket.\n"); 
		nl_close(nl->socket);
		nl_socket_free(nl->socket);
		return -ENOLINK;
	}

	nl->id = genl_ctrl_resolve(nl->socket, "nl80211");
	if (nl->id< 0) {
		log_alert(log, "Nl80211 interface not found.\n");
		nl_close(nl->socket);
		nl_socket_free(nl->socket);
		return -ENOENT;
	}

	nl->cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!nl->cb)
	{ 
		log_alert(log, "Failed to allocate netlink callback.\n"); 
		nl_close(nl->socket);
		nl_socket_free(nl->socket);
		return ENOMEM;
	}


	nl_cb_set(nl->cb, NL_CB_VALID , NL_CB_CUSTOM, getInterfaceInfo_callback, this);
	nl_cb_set(nl->cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &(nl->result));

	return nl->id;
}

int Mesh::getStationInfo(Netlink* nl)
{
	nl->result = 1;

	struct nl_msg* msg = nlmsg_alloc();

	if (!msg) 
	{
		log_alert(log, "Failed to allocate netlink message.\n");
		return -2;
	}

	genlmsg_put(msg,
		NL_AUTO_PORT,
		NL_AUTO_SEQ,
		nl->id,
		0,
		NLM_F_DUMP,
		NL80211_CMD_GET_STATION,
		0);

	nla_put_u32(msg, NL80211_ATTR_IFINDEX, network_id);
	nl_send_auto(nl->socket, msg); 

	while (nl->result > 0) 
		nl_recvmsgs(nl->socket, nl->cb);

	nlmsg_free(msg);

	return 0;
}

int Mesh::getInterfaceInfo(Netlink* nl)
{
	nl->result = 1;

	struct nl_msg* msg = nlmsg_alloc();

	if (!msg)
	{
		log_alert(log, "Failed to allocate netlink message.\n");
		return -2;
	}

	genlmsg_put(msg,
		NL_AUTO_PORT,
		NL_AUTO_SEQ,
		nl->id,
		0,
		NLM_F_DUMP,
		NL80211_CMD_GET_INTERFACE,
		0);

	nla_put_u32(msg, NL80211_ATTR_IFINDEX, network_id);
	nl_send_auto(nl->socket, msg);

	while (nl->result > 0)
		nl_recvmsgs(nl->socket, nl->cb);

	nlmsg_free(msg);

	return 0;
}

void Mesh::detected_peer(Station s)
{
	// printf("mac: \t\t%s\n"
	// 		"inactive time: \t%u\n"
	// 		"rxbyte: \t%llu\n"
	// 		"tx_packets: \t%u\n"
	// 		"signal \t\t%d\n",
	// 		s.mac_addr.c_str(), s.inactive_t, s.rx_bytes64,
	// 		s.tx_packets, s.signal);

	map<string, string>::iterator it;
	it = mapBssid.find(s.mac_addr);
	if(it != mapBssid.end())
	{
		// log_info(log, "Signal: %d\tAvg: %d\n", s.signal, s.signal_avg);
		if(s.inactive_t > LIMIT_INACTIVE_TIME_MS)
			dtn->lost_peer(s, it->second);
		else if(s.signal < MIN_SIGNAL_VALUE_DBM)
			dtn->lost_peer(s, it->second);
		else
			dtn->new_peer(s, it->second);
	}
}


void Mesh::start_monitoring(int *end)
{
	Netlink nl;

	nl.id = initNl80211(&nl);
	if (nl.id < 0)
	{
		log_alert(log, "Error initializing netlink 802.11\n");
		return;
	}

	getInterfaceInfo(&nl);

	nl_cb_set(nl.cb, NL_CB_VALID , NL_CB_CUSTOM, getStationInfo_callback, this);

	while(!*end)
	{	
		getStationInfo(&nl);
		this_thread::sleep_for(chrono::seconds(MESH_RESCAN_DELAY_SEC));
	}
	nl_cb_put(nl.cb);
	nl_close(nl.socket);
	nl_socket_free(nl.socket);
	log_info(log, "Scanning mesh networks is done\n");
}

Log& Mesh::get_log()
{
	return log;
}

/*
 * @NL80211_STA_INFO_INACTIVE_TIME: time since last activity (u32, msecs)
 * @NL80211_STA_INFO_RX_BYTES64: total received bytes (MPDU length)
 * @NL80211_STA_INFO_TX_PACKETS: total transmitted packets (MSDUs and MMPDUs)
 * @NL80211_STA_INFO_SIGNAL: signal strength of last received PPDU (u8, dBm)
 * @NL80211_STA_INFO_SIGNAL_AVG: signal strength average (u8, dBm)
*/

int getStationInfo_callback(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = (struct genlmsghdr*)nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	Mesh *mesh = (Mesh*)arg;
	Station s;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);

	if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
		tb[NL80211_ATTR_STA_INFO],
		station_policy)) 
	{
		log_alert(mesh->get_log(), "failed to parse nested attributes!\n");
		return NL_SKIP;
	}

	mac_addr_n2a(s.mac_addr, (unsigned char*)nla_data(tb[NL80211_ATTR_MAC]));

	if(sinfo[NL80211_STA_INFO_INACTIVE_TIME])
		s.inactive_t = nla_get_u32(sinfo[NL80211_STA_INFO_INACTIVE_TIME]);
	else
		log_alert(mesh->get_log(), "Failed to fetch INFO_INACTIVE_TIME !\n");
	if(sinfo[NL80211_STA_INFO_RX_BYTES64])
		s.rx_bytes64 = nla_get_u64(sinfo[NL80211_STA_INFO_RX_BYTES64]);
	else
		log_alert(mesh->get_log(), "Failed to fetch INFO_RX_BYTES64 !\n");
	if(sinfo[NL80211_STA_INFO_TX_PACKETS])
		s.tx_packets = nla_get_u32(sinfo[NL80211_STA_INFO_TX_PACKETS]);
	else
		log_alert(mesh->get_log(), "Failed to fetch INFO_TX_PACKETS !\n");
	if(sinfo[NL80211_STA_INFO_SIGNAL])
		s.signal = (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
	else
		log_alert(mesh->get_log(), "Failed to fetch INFO_SIGNAL !\n");
	if(sinfo[NL80211_STA_INFO_SIGNAL_AVG])
		s.signal_avg = (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL_AVG]);
	else
		log_alert(mesh->get_log(), "Failed to fetch INFO_SIGNAL_AVG !\n");

	mesh->detected_peer(s);

	return NL_SKIP;
}

int getInterfaceInfo_callback(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = (struct genlmsghdr*)nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	Mesh *mesh = (Mesh*)arg;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);


	if (tb[NL80211_ATTR_IFTYPE])
		log_info(mesh->get_log(), "Type: %s\n",
					iftype_name(nla_get_u32(tb[NL80211_ATTR_IFTYPE])));
	else
		log_alert(mesh->get_log(), "Failed to fetch NL80211_ATTR_IFTYPE !\n");

	if (tb[NL80211_ATTR_WIPHY_FREQ])
		log_info(mesh->get_log(), "Freq: %u\n", nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]));
	else
		log_alert(mesh->get_log(), "Failed to fetch NL80211_ATTR_WIPHY_FREQ !\n");

	if (tb[NL80211_ATTR_WIPHY_TX_POWER_LEVEL]) {
		uint32_t txp = nla_get_u32(tb[NL80211_ATTR_WIPHY_TX_POWER_LEVEL]);

		log_info(mesh->get_log(), "Txpower: %d.%.2d dBm\n", txp / 100, txp % 100);
	}
	else
		log_alert(mesh->get_log(), "Failed to fetch NL80211_ATTR_WIPHY_TX_POWER_LEVEL !\n");

	return NL_SKIP;
}

int finish_handler(struct nl_msg *msg, void *arg)
{
	(void)msg;
	int *ret = (int*)arg;
	*ret = 0;
	return NL_SKIP;
}

const char *iftype_name(uint32_t iftype)
{
	if (iftype <= NL80211_IFTYPE_MAX && ifmodes[iftype])
		return ifmodes[iftype];
	return "Unknown";
}