#!/bin/bash

rm /media/ws/rootfs/home/tt/TT/main_arm

cp /home/ws/project/phascan/Phascan/build/develop-mini-arm/main_arm /media/ws/rootfs/home/tt/TT

md5sum /home/ws/project/phascan/Phascan/build/develop-mini-arm/main_arm /media/ws/rootfs/home/tt/TT/main_arm /home/ws/project/phascan/release/rootfs/home/tt/TT/main_arm
