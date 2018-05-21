DIR="fetched/"
PREF_NAME="MobRPi"
FILE="out.txt"
NB_SEND=""
NB_RECV=""
AVG_SIGN=""

#Merging files 
sort "$DIR"*.txt > "$DIR$FILE"

##############################
########### STATS  ###########
##############################

TYPE=$(grep -o -m1 "Type.*" $DIR$FILE)
echo $TYPE
echo $(grep -o -m1 "Freq.*" $DIR$FILE)
echo $(grep -o -m1 "Txpower.*" $DIR$FILE)
echo "Brd type: "$(grep -o -m1 "BRD_.*" $DIR$FILE)
echo

for i in $@; do
	NB_SEND=$(grep $PREF_NAME$i $DIR$FILE | grep -c SEND)
	INTERVAL=$(grep $PREF_NAME$i $DIR$FILE | grep -o [0-9]*ms)
	echo "$PREF_NAME$i sent $NB_SEND brd with interval of $INTERVAL"
	if [ "$NB_SEND" -gt 0 ]; then
		for j in $@; do
			if [ "$i" != "$j" ]; then
				NB_RECV=$(grep $PREF_NAME$j $DIR$FILE | grep -c "from 192.168.2."$i"")
				AVG_SIGN=$(grep $PREF_NAME$j $DIR$FILE | grep "from 192.168.2."$i"" | grep -o "[\-][0-9]*\ dbm" | grep -o "\-[0-9]*" | awk '{sum+=$NF} END{print sum/NR}')
				PERC=$(bc <<< 'scale=2; '$NB_RECV*100/$NB_SEND'')
				echo -e "\t$PREF_NAME$j received $PERC% of them - avg of $AVG_SIGN dbm"
			fi
		done
	fi
done
echo -e "\n################\n"

##############################
########### GRAPH  ###########
##############################

GRAPHTITLE="$TYPE \/ $INTERVAL \/ $NB_SEND brd"
GRAPHDIR="graph/"
firstTs=$(grep -o -m1 "[0-9]\{13\}" $DIR$FILE)
filename="$GRAPHDIR$firstTs".tex
touch "$filename"

sed "s/GRAPHNAME/$GRAPHTITLE/g" "$GRAPHDIR"pref.tex > "$filename"

for i in $@; do
	ts_rpi=$(grep MobRPi$i $DIR$FILE | grep SEND | grep -o -m1 "[0-9]\{13\}")
	for j in $@; do
		cpt=0
		if [ "$i" != "$j" ]; then
			echo "\addplot coordinates {" >> "$filename"

			while read -r ts ; do
				let "cpt += 1"
				echo "( $(($ts - ts_rpi)), $cpt)" >> "$filename"
			done < <(grep MobRPi$j $DIR$FILE | grep "from 192.168.2.$i" | grep -m20 -o "[0-9]\{13\}")

			echo "};" >> "$filename"
			echo "\addlegendentry{$i \$\rightarrow\$ $j}" >> "$filename"
		fi
	done
done

cat "$GRAPHDIR"suff.tex >> "$filename"

cd "$GRAPHDIR"
pdflatex "$firstTs".tex > /dev/null
rm *.aux*
rm *.log
cd ..

evince "$GRAPHDIR$firstTs.pdf"