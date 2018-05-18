#! /bin/bash
chToMhz[1]='2412'
chToMhz[2]='2417'
chToMhz[3]='2422'
chToMhz[4]='2427'
chToMhz[5]='2432'
chToMhz[6]='2437'
chToMhz[7]='2442'
chToMhz[8]='2447'
chToMhz[9]='2452'
chToMhz[10]='2457'
chToMhz[11]='2462'
chToMhz[12]='2467'
chToMhz[13]='2472'
chToMhz[14]='2484'

function usage(){
	echo Usage: ./${0##*/} start \{interface\} \{channel\}
	echo Usage: ./${0##*/} stop \{interface\}
}

ID="1"
SSID="ADHOC"
MHZ_CHANNEL="0"

if [ $# -eq 0 ];
then
	usage
	exit 1
fi

DEV=$2

if [ "$1" == "start" ] && [ $# -eq 3 ];then
	MHZ_CHANNEL=${chToMhz["$3"]}
	if [ -z "$MHZ_CHANNEL" ];then
		echo "Wrong channel number specified"
		exit 1
	fi
	echo "$2: setting up adhoc network \"$SSID\" on channel $3 ($MHZ_CHANNEL mhz)"
	ip link set $DEV down
	iw $DEV set type ibss
	ip link set $DEV up
	iw $DEV ibss join $SSID $MHZ_CHANNEL

	ip addr add 192.168.2."$ID"/24 dev $DEV
	ip route add 192.168.2.0/24 dev $DEV

elif [ "$1" == "stop" ] && [ $# -eq 2 ];then
	echo "Leaving ibss \"$SSID\""
	iw $DEV ibss leave
	echo "Removing ip 192.168.2.$ID/24 from $DEV"
	ip addr del 192.168.2."$ID"/24 dev $DEV
	echo "Removing route 192.168.2.0/24 on $DEV"
	ip route del 192.168.2.0/24 dev $DEV

else
	usage
	exit 1
fi