#!/bin/sh

# SD卡有多大容量
sudo dd if=/dev/zero of=xrz.img bs=1M count=7580

# 开始对SD卡分区
# 磁头255 扇区63 柱面966
# 柱面966=7580*1024(bs=1M)/255/63/512
# 公式：柱面=容量(单位byte) / 磁头 / 扇区 / 512
# 格式 <start>，<size>，<type [E,S,L,X,hex]>，<bootable [-,*]>
# 第一个分区，从0开始 9个柱面 类型0x0C bootable
# 第二个分区，从上次的哪个柱面开始 剩下所有的柱面(即容量) 其他的参数全部默认
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
