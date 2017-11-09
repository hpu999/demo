#!/bin/sh
# This script creates (pre-)release zipballs with their md5sums.

VERSION=`cat rootfs/etc/version`
echo "Making zipballs for version $VERSION..."

#git archive --format=tar --prefix=Phascan-$VERSION/ HEAD | xz > Phascan-$VERSION.tar.xz
#sha256sum Phascan-$VERSION.tar.xz > Phascan-$VERSION.tar.xz.sha256sum

COUNT=1843		# 单位(M)

IMG_FILE=Phascan-${VERSION}.img

dd if=/dev/zero of=${IMG_FILE} bs=1M count=$COUNT

SIZE=`echo 1024*1024*$COUNT | bc`

CYLINDERS=`echo $SIZE/255/63/512 | bc`

sfdisk -D -H 255 -S 63 -C $CYLINDERS ${IMG_FILE} << EOF 
,9,0x0C,*
,,,-
EOF

DRIVE=`losetup -f`
DRIVE_NAME=`basename $DRIVE`
DEV_DIR=`dirname $DRIVE`

losetup $DRIVE ${IMG_FILE}
kpartx -av $DRIVE

sudo mkfs.vfat -F 32 -n "boot" $DEV_DIR/mapper/${DRIVE_NAME}p1

sudo mke2fs -j -L "rootfs" $DEV_DIR/mapper/${DRIVE_NAME}p2

sudo mkdir /mnt/boot
sudo mkdir /mnt/rootfs

sudo mount $DEV_DIR/mapper/${DRIVE_NAME}p1 /mnt/boot
sudo mount $DEV_DIR/mapper/${DRIVE_NAME}p2 /mnt/rootfs

sudo cp boot/*   /mnt/boot   -rfd
sudo cp rootfs/* /mnt/rootfs -rfd

sync

sudo umount /mnt/boot
sudo umount /mnt/rootfs

sudo kpartx  -d $DRIVE
sudo losetup -d $DRIVE

sudo rm /mnt/boot -R
sudo rm /mnt/rootfs -R

zip -r Phascan-${VERSION}.zip ${IMG_FILE}
rm ${IMG_FILE} -f

echo "Done."
