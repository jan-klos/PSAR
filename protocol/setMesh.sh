#!/bin/bash

function usage(){
	echo Usage: ./${0##*/} start \{interface\} \{channel\}
	echo Usage: ./${0##*/} stop \{interface\}
}

#changer id pour chaque rpi
ID="2"
MESHNAME="MOBMESH"

if [ $# -eq 0 ];
then
	usage
	exit 1
fi

DEV=$2

if [ "$1" == "start" ] && [ $# -eq 3 ];then
	echo "$2: setting up mesh network \"$MESHNAME\" on channel $3"
	ip link set $DEV down
	iw $DEV set type mp
	ip link set $DEV up
	iw $DEV set channel $3
	iw $DEV mesh join $MESHNAME

	ip addr add 192.168.2."$ID"/24 dev $DEV
	ip route add 192.168.2.0/24 dev $DEV

elif [ "$1" == "stop" ] && [ $# -eq 2 ];then
	echo "Leaving mesh \"$MESHNAME\""
	iw $DEV mesh leave
	echo "Removing ip 192.168.2.$ID/24 from $DEV"
	ip addr del 192.168.2."$ID"/24 dev $DEV
	echo "Removing route 192.168.2.0/24 on $DEV"
	ip route del 192.168.2.0/24 dev $DEV

else
	usage
	exit 1
fi

echo "Done !"
