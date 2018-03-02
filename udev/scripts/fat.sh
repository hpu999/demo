#!/bin/sh

/etc/udev/scripts/mount.sh

cat /proc/mounts | awk '{print $1}' | grep -q "^$DEVNAME$" && /opt/TestUSB
