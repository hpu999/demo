#!/bin/bash

BASE_ADDR0=0x2f000000
BASE_ADDR1=0x2f040000
BASE_ADDR2=0x2f080000
BASE_ADDR3=0x2f0c0000

for i in `seq 0 15`
do
	num=`expr ${i} % 4`
	eval echo "\$BASE_ADDR${num}"
	sleep 1
done
