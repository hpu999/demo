#!/bin/bash

grep -n "\bif\b" $1 | awk -F: '{print $1}' > .tmp
n=`wc -l .tmp | awk '{print $1}'`

/bin/cp $1 .cache1
for i in `seq 1 $n`
do
	line=`sed -n "${i}p" .tmp`
	string=`sed -n "${line}p" .cache1`
	sed "${line}a ########### $string" .cache1 > .cache2
	> .cache1
	sed "${line}d" .cache2 > .cache1
	> .cache2
done

rm $1 
/bin/cp .cache1 $1
rm .tmp .cache*
