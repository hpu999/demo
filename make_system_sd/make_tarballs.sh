#!/bin/sh
# This script creates (pre-)release tarballs with their md5sums.
# To use this script:
# 1) Change Phascan Version in README.md to the final release version that you are making.
# 2) Call this script to create the tarballs.

VERSION=`cat rootfs/etc/version`
echo "Making tarballs for version $VERSION..."

git archive --format=tar --prefix=Phascan-$VERSION/ HEAD | xz > Phascan-$VERSION.tar.xz
sha256sum Phascan-$VERSION.tar.xz > Phascan-$VERSION.tar.xz.sha256sum

echo "Done."
