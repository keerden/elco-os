#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/elco-os.kernel isodir/boot/elco-os.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "elco-os" {
	multiboot /boot/elco-os.kernel
}
EOF
grub-mkrescue -o elco-os.iso isodir
