#!/bin/bash

function usage() {
	echo Usage: ./${0##*/} \<rpi_alias1\> ... \<rpi_aliasN\>
}

if [ $# -eq 0 ]; 
then
	usage
	exit 1
fi

mv fetched/*.txt fetched/history/

for i in $@; do
	LASTLOG=""
	LASTLOG=$(ssh "$i" ls dev/code/log | tail -n1)
	./achieve.sh fetch /home/pi/dev/code/log/"$LASTLOG" "$i"
done