#!/bin/bash

g++ -lpthread $1
echo "testing thread spawning" #picka
for i in {0..4}; do
	echo -e "\ttry $i spawns"
	OUT=`./a.out $i | head -n 1`
	EXP_OUT=$((i+1))
	if [ $OUT -eq $EXP_OUT ]; then
  		echo -e "\t\tok"
	else
		echo -e "\t\tfailed"
	fi
done

echo "testing curl retrieval"
BLOG=`curl -s https://www.result.si/blog/`
KARIERA=`curl -s https://www.result.si/kariera/`
ONAS=`curl -s https://www.result.si/o-nas/`
PROJEKTI=`curl -s https://www.result.si/projekti/`
for i in {0..4}; do
	ERR=0 #errors
	echo -e "\twith $i spawns"
	OUT=`./a.out $i` #contains dump of all 4 curls
	echo $OUT
	exit
	if ! [[ $OUT == *$BLOG* ]]; then #if $OUT doesn't contain curl
		echo -e "\t\tfailed BLOG"
		ERR=$((ERR+1))
	fi
	if ! [[ $OUT == *$KARIERA* ]]; then #if $OUT doesn't contain curl
		echo -e "\t\tfailed KARIERA"
		ERR=$((ERR+1))
	fi
	if ! [[ $OUT == *$ONAS* ]]; then #if $OUT doesn't contain curl
		echo -e "\t\tfailed O-NAS"
		ERR=$((ERR+1))
	fi
	if ! [[ $OUT == *$PROJEKTI* ]]; then #if $OUT doesn't contain curl
		echo -e "\t\tfailed PROJEKTI"
		ERR=$((ERR+1))
	fi
	if [ $ERR -eq 0 ]; then echo -e "\t\tok"; fi #no errors
done
