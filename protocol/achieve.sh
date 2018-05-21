#!/bin/bash

function usage() {
	echo Usage: ./${0##*/} \[send\|fetch\] \<pathToFile\> \<rpi_alias1\> ... \<rpi_aliasN\>
	echo Usage: ./${0##*/} \[\exec] \<\command\> \<rpi_alias1\> ... \<rpi_aliasN\>
}

if [ $# -eq 0 ]; 
then
	usage
	exit 1
fi

RECURSIVE="" 
if [ "$1" != "send" ] && [ "$1" != "fetch" ] && [ "$1" != "exec" ]; then
	usage
	exit 1
fi

if [ "$1" == "send" ]; then
	if [ ! -f $2 ] && [ ! -d $2 ]; then
		echo "$2 not found!"
		exit 1
	fi
	if [[ -d $2 ]]; then
		echo "Is a directory"
		RECURSIVE=" -r "
	else
		echo "Is a file"
		RECURSIVE=""
	fi
fi


for i in "$@"; do
	if [ "$i" == "$1" ] || [ "$i" == "$2" ]; then
		continue
	fi
	if [ "$1" == "send" ]; then
		echo "Sending $2 on $i"
		scp -q $RECURSIVE $2 $i:/home/pi/dev
	elif [ "$1" == "fetch" ]; then
		echo "Fetching $2 from $i..."
		scp $RECURSIVE $i:$2 fetched/
	elif [ "$1" == "exec" ]; then
		echo "Running \"$2\" on $i..."
		ssh $i $2
	else
		echo "Error \$1 unknown: $1"
	fi
done