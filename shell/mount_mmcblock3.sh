#!/bin/sh

PARTTION_TOOL="/usr/bin/parttion"

if [ -e "${PARTTION_TOOL}" ] ; then
	${PARTTION_TOOL}
	sync
	sleep 1
	umount /media/mmcblk0p3
	mke2fs -j -L "doppler" /dev/mmcblk0p3
	while [ $? -ne 0 ] ; do
		sleep 1
		mke2fs -j -L "doppler" /dev/mmcblk0p3
	done
	sync
fi

/bin/umount /media/mmcblk0p3
/bin/mount /dev/mmcblk0p3 /home/tt/TT/data/
while [ $? -ne 0 ] ; do
	sleep 1
	/bin/umount /media/mmcblk0p3
	/bin/mount /dev/mmcblk0p3 /home/tt/TT/data/
done

if [ -e "${PARTTION_TOOL}" ] ; then
	rm /home/tt/TT/data/* -r
	cp -rf /home/tt/TT/data.bak/* /home/tt/TT/data/
	rm ${PARTTION_TOOL}
fi
