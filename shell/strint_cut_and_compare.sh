#!/bin/bash

DRIVER=$1
LAST_NUM=`echo $DRIVER | wc -L`
LAST_NUM1=$LAST_NUM-1 
DRIVER1=${DRIVER:LAST_NUM1:LAST_NUM}
DRIVER2=${DRIVER:0:LAST_NUM1}

if [ "/dev/sd" == "${DRIVER:0:7}" ]
then
	if echo "0123456789" | grep -q "${DRIVER1}"
	then
		for DEV in `ls $DRIVER2[0-9]`
		do
			if [ -e $DEV ]
			then
				if [ "$DEV" == "$DRIVER" ]
				then
					echo "$DEV $DRIVER"
					echo "mount"
					break
				else
					echo "$DEV $DRIVER"
					echo "no mount"
					exit
				fi
			fi
		done
	fi
else
	echo "1111111"
fi
