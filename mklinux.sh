#!/usr/bin/env bash

mkdir -p build

if [ -z "$URL" ]
then
	export URL="https://github.com/revyos/thead-kernel"
fi

if [ -z "$BRANCH" ]
then
	export BRANCH="lpi4a"
fi

if [ -z "$COMMIT" ]
then
	export COMMIT="b8c5d354603d9833a4ee89bf0473f0753cad168f"
fi

if [ -z "$CROSS_COMPILE" ]
then
	# toolchain from vendor
	export CROSS_COMPILE="riscv64-unknown-linux-gnu-"
	# generic toolchain
	#export CROSS_COMPILE="riscv64-linux-gnu-"
fi

if [ -z "$CONFIG" ]
then
	export CONFIG="revyos_defconfig"
fi

set -eux

if [ ! -e build/linux ] 
then
	git clone $URL build/linux --branch=${BRANCH}
	cd build/linux
	git reset --hard $COMMIT
	find ../../linux/ -name *.patch | sort | while read line
	do
		git am $line
	done
	cd ../../
fi

cd build/linux
export ARCH=riscv
export INSTALL_PATH=$(pwd)/_install/boot/
export INSTALL_MOD_PATH=$(pwd)/_install/
if [ -e ${INSTALL_PATH} ]
then
	rm -rf ${INSTALL_PATH}
fi
mkdir -p ${INSTALL_PATH}
make mrproper
make $CONFIG
make -j$(nproc)
make install
make dtbs_install
make modules_install
cd ${INSTALL_PATH}/
cp $(ls vmlinuz-* | grep -v old) Image
cd dtbs
cp -r ./*/thead ./
cd ../../../
cp -r _install/boot ../../overlay/
cp -r _install/lib/modules ../../overlay/usr/lib/
cp -v vmlinux ../../overlay/opt/vmlinux
