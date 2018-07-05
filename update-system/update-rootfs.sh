#!/bin/sh

MOUNT=/bin/mount
UMOUNT=/bin/umount
LOSETUP=/sbin/losetup
MKDIR=/bin/mkdir
RM=/bin/rm
CP=/bin/cp

MOUNT_DIR=/system

update_rootfs () {
	
	$RM  $MOUNT_DIR/etc    -rf
	$RM  $MOUNT_DIR/bin    -rf
	$RM  $MOUNT_DIR/home   -rf
	$RM  $MOUNT_DIR/opt    -rf
#	$RM  $MOUNT_DIR/media  -rf  # 挂载目录 外部存储的数据 不能删除
	$RM  $MOUNT_DIR/usr    -rf
	$RM  $MOUNT_DIR/boot   -rf
	$RM  $MOUNT_DIR/lib    -rf
	$RM  $MOUNT_DIR/mnt    -rf
	$RM  $MOUNT_DIR/sbin   -rf
#	$RM  $MOUNT_DIR/dev    -rf
#	$RM  $MOUNT_DIR/run    -rf
#	$RM  $MOUNT_DIR/sys    -rf
#	$RM  $MOUNT_DIR/proc   -rf
#	$RM  $MOUNT_DIR/tmp    -rf
#	$RM  $MOUNT_DIR/var    -rf

	$CP /etc   $MOUNT_DIR/  -rfd
	$CP /bin   $MOUNT_DIR/  -rfd
	$CP /home  $MOUNT_DIR/  -rfd
	$CP /opt   $MOUNT_DIR/  -rfd
	$CP /usr   $MOUNT_DIR/  -rfd
	$CP /boot  $MOUNT_DIR/  -rfd
	$CP /lib   $MOUNT_DIR/  -rfd
	$CP /mnt   $MOUNT_DIR/  -rfd
	$CP /sbin  $MOUNT_DIR/  -rfd
	$CP /media $MOUNT_DIR/  -rfd
#	$CP /dev   $MOUNT_DIR/  -rfd
#	$CP /run   $MOUNT_DIR/  -rfd
#	$CP /sys   $MOUNT_DIR/  -rfd
#	$CP /proc  $MOUNT_DIR/  -rfd
#	$CP /tmp   $MOUNT_DIR/  -rfd
#	$CP /var   $MOUNT_DIR/  -rfd

	/bin/sync
}

! test -d "$MOUNT_DIR" && $MKDIR $MOUNT_DIR

$MOUNT /dev/mmcblk2p2 $MOUNT_DIR

update_rootfs

$UMOUNT $MOUNT_DIR

$RM $MOUNT_DIR -rf
