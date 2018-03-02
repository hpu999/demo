#!/bin/sh

crossPath="/stuff/bitbake/bin/tmp/sysroots/i686-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-"

if [ ! -f "./.config" ]; then
	cp ./configs/uImage.config ./.config
	if [ -f "./.config" ]; then
		make -j4 CROSS_COMPILE=$crossPath  ARCH=arm uImage
	else 
		echo "No .config file."
	fi
else 
	make -j4 CROSS_COMPILE=$crossPath  ARCH=arm uImage
fi

