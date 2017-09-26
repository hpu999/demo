#!/bin/bash

for i in `find ./ -name "build"`
do
	scp -r $i/bin/* root@192.168.1.4:/usr/bin 
	scp -r $i/lib/* root@192.168.1.4:/usr/lib 
	scp -r $i/lib/* root@192.168.1.4:/usr/lib 
	scp -r $i/include/* root@192.168.1.4:/usr/include 
	scp -r $i/share/* root@192.168.1.4:/usr/share 
done

echo "oK!"
