#!/bin/sh

sudo dd if=/dev/zero of=xrz.img bs=1M count=7580

sudo sfdisk -D -H 255 -S 63 -C 966 xrz.img << EOF 
,9,0x0C,*
,,,-
EOF

DRIVE=`losetup -f`
DRIVE_NAME=`basename $DRIVE`
DEV_DIR=`dirname $DRIVE`

sudo losetup $DRIVE xrz.img
sudo kpartx -av $DRIVE

sudo mkfs.vfat -F 32 -n "boot" $DEV_DIR/mapper/${DRIVE_NAME}p1

sudo mke2fs -j -L "rootfs" $DEV_DIR/mapper/${DRIVE_NAME}p2

sudo mkdir /mnt/boot
sudo mkdir /mnt/rootfs

sudo mount $DEV_DIR/mapper/${DRIVE_NAME}p1 /mnt/boot
sudo mount $DEV_DIR/mapper/${DRIVE_NAME}p2 /mnt/rootfs

sudo cp /home/ws/project/phascan/release/boot/*   /mnt/boot   -rfd
sudo cp /home/ws/project/phascan/release/rootfs/* /mnt/rootfs -rfd

sync

sudo umount /mnt/boot
sudo umount /mnt/rootfs

sudo kpartx  -d $DRIVE
sudo losetup -d $DRIVE

sudo rm /mnt/boot -R
sudo rm /mnt/rootfs -R

zip -r $1 xrz.img
