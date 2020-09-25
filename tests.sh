#!/bin/bash

#compile
g++ -lpthread naloga.cpp -o naloga.out

#test 1
echo "testing wrong arguments"
if [[ `./naloga.out -3` == "" ]]; then
	echo -e "\tok"
fi
if [[ `./naloga.out 6` == "" ]]; then
	echo -e "\tok"
fi
if [[ `./naloga.out kek` == `./naloga.out 0` ]]; then
	echo -e "\tok"
fi
echo "testing wrong number of arguments"
if [[ `./naloga.out 1 kek` == "" ]]; then
	echo -e "\tok"
fi

#test 2
echo "testing thread spawning"
for i in {0..4}; do #for each arg
	echo -e "\ttry $i spawns"
	OUT=`./naloga.out $i | head -n 1` #get first line of output
	EXP_OUT=$((i+1))
	if [ $OUT -eq $EXP_OUT ]; then #compare to $EXP_OUT
  		echo -e "\t\tok"
	else
		echo -e "\t\tfailed"
	fi
done

#cleanup
shred -zun 6 naloga.out

##test 3
#echo "testing curl retrieval"
#BLOG=`curl -s https://www.result.si/blog/`
#KARIERA=`curl -s https://www.result.si/kariera/`
#ONAS=`curl -s https://www.result.si/o-nas/`
#PROJEKTI=`curl -s https://www.result.si/projekti/`
#for i in {0..4}; do
#	ERR=0 #errors
#	echo -e "\twith $i spawns"
#	OUT=`./naloga.out $i` #contains dump of all 4 curls
#	echo $OUT
#	exit
#	if ! [[ $OUT == *$BLOG* ]]; then #if $OUT doesn't contain curl
#		echo -e "\t\tfailed BLOG"
#		ERR=$((ERR+1))
#	fi
#	if ! [[ $OUT == *$KARIERA* ]]; then #if $OUT doesn't contain curl
#		echo -e "\t\tfailed KARIERA"
#		ERR=$((ERR+1))
#	fi
#	if ! [[ $OUT == *$ONAS* ]]; then #if $OUT doesn't contain curl
#		echo -e "\t\tfailed O-NAS"
#		ERR=$((ERR+1))
#	fi
#	if ! [[ $OUT == *$PROJEKTI* ]]; then #if $OUT doesn't contain curl
#		echo -e "\t\tfailed PROJEKTI"
#		ERR=$((ERR+1))
#	fi
#	if [ $ERR -eq 0 ]; then echo -e "\t\tok"; fi #no errors
#done
