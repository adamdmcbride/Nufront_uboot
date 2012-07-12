#!/bin/bash

if [ $1 ]
then
	make distclean
	rm -rf uboot.img
	USE_PRIVATE_LIBGCC=yes make "$1_config"
	USE_PRIVATE_LIBGCC=yes make -j64
	echo "rename u-boot.bin to u-boot-$1.bin"
	mv "u-boot.bin" "u-boot-$1.bin"
else
	USE_PRIVATE_LIBGCC=yes make -j64
	btype="$(cat include/config.mk | grep BOARD)"
	echo "rename u-boot.bin to u-boot-${btype:9}.bin"
	mv "u-boot.bin" "u-boot-${btype:9}.bin"
fi	
./tools/add_head u-boot uboot.img


