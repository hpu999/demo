#!/bin/sh
# This script creates (pre-)release zipballs with their md5sums.

VERSION=`cat rootfs/etc/version`
echo "Making update zipballs for version $VERSION..."

COUNT=512		# 单位(M)

IMG_FILE=Update.img

dd if=/dev/zero of=${IMG_FILE} bs=1M count=$COUNT

SIZE=`echo 1024*1024*$COUNT | bc`

CYLINDERS=`echo $SIZE/255/63/512 | bc`

sfdisk -D -H 255 -S 63 -C $CYLINDERS ${IMG_FILE} << EOF 
,,,-
EOF

DRIVE=`losetup -f`

losetup $DRIVE ${IMG_FILE}

sudo mke2fs $DRIVE

sudo mkdir /mnt/system

sudo mount $DRIVE /mnt/system

sudo mkdir /mnt/system/boot
sudo mkdir /mnt/system/rootfs

sudo cp boot/*   /mnt/system/boot   -rfd
sudo cp rootfs/* /mnt/system/rootfs -rfd

sync

sudo umount /mnt/system

sudo losetup -d $DRIVE

sudo rm /mnt/system -R

zip -r Flexscan-update-${VERSION}.doppler ${IMG_FILE}
rm ${IMG_FILE} -f

echo "Done."
