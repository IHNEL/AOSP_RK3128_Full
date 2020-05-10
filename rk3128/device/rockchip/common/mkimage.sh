#!/bin/bash
set -e

. build/envsetup.sh >/dev/null && setpaths

export PATH=$ANDROID_BUILD_PATHS:$PATH
TARGET_PRODUCT=`get_build_var TARGET_PRODUCT`
TARGET_HARDWARE=`get_build_var TARGET_BOARD_HARDWARE`
TARGET_BOARD_PLATFORM=`get_build_var TARGET_BOARD_PLATFORM`
PLATFORM_VERSION=`get_build_var PLATFORM_VERSION`
PLATFORM_SECURITY_PATCH=`get_build_var PLATFORM_SECURITY_PATCH`
TARGET_BUILD_VARIANT=`get_build_var TARGET_BUILD_VARIANT`
BOARD_SYSTEMIMAGE_PARTITION_SIZE=`get_build_var BOARD_SYSTEMIMAGE_PARTITION_SIZE`
BOARD_USE_SPARSE_SYSTEM_IMAGE=`get_build_var BOARD_USE_SPARSE_SYSTEM_IMAGE`
echo TARGET_BOARD_PLATFORM=$TARGET_BOARD_PLATFORM
echo TARGET_PRODUCT=$TARGET_PRODUCT
echo TARGET_HARDWARE=$TARGET_HARDWARE
echo TARGET_BUILD_VARIANT=$TARGET_BUILD_VARIANT
echo BOARD_SYSTEMIMAGE_PARTITION_SIZE=$BOARD_SYSTEMIMAGE_PARTITION_SIZE
echo BOARD_USE_SPARSE_SYSTEM_IMAGE=$BOARD_USE_SPARSE_SYSTEM_IMAGE
TARGET="withoutkernel"
if [ "$1"x != ""x  ]; then
         TARGET=$1
fi

IMAGE_PATH=rockdev/Image-$TARGET_PRODUCT
UBOOT_PATH=u-boot
KERNEL_PATH=kernel
rm -rf $IMAGE_PATH
mkdir -p $IMAGE_PATH

FSTYPE=ext4
echo system filesysystem is $FSTYPE

BOARD_CONFIG=device/rockchip/common/device.mk
if [[ $TARGET_BOARD_PLATFORM = "rk3399" ]]; then
	if [[ $BOARD_SYSTEMIMAGE_PARTITION_SIZE = "1610612736" ]]; then
		PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/$TARGET_PRODUCT/parameter_system_1.5G.txt
	elif [[ $BOARD_SYSTEMIMAGE_PARTITION_SIZE = "3221225472" ]]; then
		PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/$TARGET_PRODUCT/parameter_system_3.0G.txt
	elif [[ $BOARD_SYSTEMIMAGE_PARTITION_SIZE = "4294967296" ]]; then
		PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/$TARGET_PRODUCT/parameter_system_4.0G.txt
	else
		PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/$TARGET_PRODUCT/parameter.txt
	fi
elif [[ $TARGET_BOARD_PLATFORM = "rk3368" ]]; then
	if [[ $BOARD_SYSTEMIMAGE_PARTITION_SIZE = "1610612736" ]]; then
                PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/parameter_system_1.5G.txt
        elif [[ $BOARD_SYSTEMIMAGE_PARTITION_SIZE = "3221225472" ]]; then
                PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/parameter_system_3.0G.txt
        elif [[ $BOARD_SYSTEMIMAGE_PARTITION_SIZE = "4294967296" ]]; then
                PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/parameter_system_4.0G.txt
        else
                PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/parameter.txt
        fi
elif [[ $TARGET_BOARD_PLATFORM = "rk3126c" ]]; then
    PARAMETER=device/rockchip/rk312x/rk3126c/parameter.txt
elif [[ $TARGET_BOARD_PLATFORM = "rk3128" ]]; then
    PARAMETER=device/rockchip/rk312x/rk3128_box/parameter.txt
elif [[ $TARGET_BOARD_PLATFORM = "px3se" ]]; then
    PARAMETER=device/rockchip/rk312x/px3se/parameter.txt
else
	if [[ $TARGET_PRODUCT = "px5" || $TARGET_PRODUCT = "px3" ]]; then
		PARAMETER=device/rockchip/$TARGET_PRODUCT/parameter.txt
	else
		PARAMETER=device/rockchip/$TARGET_BOARD_PLATFORM/parameter.txt
	fi
fi

KERNEL_SRC_PATH=`grep TARGET_PREBUILT_KERNEL ${BOARD_CONFIG} |grep "^\s*TARGET_PREBUILT_KERNEL *:= *[\w]*\s" |awk  '{print $3}'`

[ $(id -u) -eq 0 ] || FAKEROOT=fakeroot

BOOT_OTA="ota"

[ $TARGET != $BOOT_OTA -a $TARGET != "withoutkernel" ] && echo "unknow target[${TARGET}],exit!" && exit 0

    if [ ! -f $OUT/kernel ]
    then
	    echo "kernel image not fount![$OUT/kernel] "
        read -p "copy kernel from TARGET_PREBUILT_KERNEL[$KERNEL_SRC_PATH] (y/n) n to exit?"
        if [ "$REPLY" == "y" ]
        then
            [ -f $KERNEL_SRC_PATH ]  || \
                echo -n "fatal! TARGET_PREBUILT_KERNEL not eixit! " || \
                echo -n "check you configuration in [${BOARD_CONFIG}] " || exit 0

            cp ${KERNEL_SRC_PATH} $OUT/kernel

        else
            exit 0
        fi
    fi

if [ $TARGET == $BOOT_OTA ]
then
	echo "make ota images... "
	echo -n "create boot.img with kernel... "
	[ -d $OUT/root ] && \
	mkbootfs $OUT/root | minigzip > $OUT/ramdisk.img && \
        truncate -s "%4" $OUT/ramdisk.img && \
        mkbootimg --kernel $OUT/kernel --ramdisk $OUT/ramdisk.img --second kernel/resource.img --os_version $PLATFORM_VERSION --os_patch_level $PLATFORM_SECURITY_PATCH --cmdline buildvariant=$TARGET_BUILD_VARIANT --output $OUT/boot.img && \
	cp -a $OUT/boot.img $IMAGE_PATH/
	echo "done."
else
	echo -n "create boot.img without kernel... "
	[ -d $OUT/root ] && \
	mkbootfs $OUT/root | minigzip > $OUT/ramdisk.img && \
        truncate -s "%4" $OUT/ramdisk.img && \
	rkst/mkkrnlimg $OUT/ramdisk.img $IMAGE_PATH/boot.img >/dev/null
	echo "done."
fi
if [ $TARGET == $BOOT_OTA ]
then
	echo -n "create recovery.img with kernel and resource... "
	[ -d $OUT/recovery/root ] && \
	mkbootfs $OUT/recovery/root | minigzip > $OUT/ramdisk-recovery.img && \
        truncate -s "%4" $OUT/ramdisk-recovery.img && \
        mkbootimg --kernel $OUT/kernel --ramdisk $OUT/ramdisk-recovery.img --second kernel/resource.img --os_version $PLATFORM_VERSION --os_patch_level $PLATFORM_SECURITY_PATCH --cmdline buildvariant=$TARGET_BUILD_VARIANT --output $OUT/recovery.img && \
	cp -a $OUT/recovery.img $IMAGE_PATH/
	echo "done."
else
	echo -n "create recovery.img without kernel and resource... "
	[ -d $OUT/recovery/root ] && \
	mkbootfs $OUT/recovery/root | minigzip > $OUT/ramdisk-recovery.img && \
        truncate -s "%4" $OUT/ramdisk-recovery.img && \
        #mkbootimg --kernel $OUT/kernel --ramdisk $OUT/ramdisk-recovery.img --os_version $PLATFORM_VERSION --os_patch_level $PLATFORM_SECURITY_PATCH --cmdline buildvariant=$TARGET_BUILD_VARIANT --output $OUT/recovery.img && \
        rkst/mkkrnlimg $OUT/ramdisk-recovery.img $OUT/recovery.img
	cp -a $OUT/recovery.img $IMAGE_PATH/
	echo "done."
fi
	echo -n "create misc.img.... "
	cp -a rkst/Image/misc.img $IMAGE_PATH/misc.img
	cp -a rkst/Image/pcba_small_misc.img $IMAGE_PATH/pcba_small_misc.img
	cp -a rkst/Image/pcba_whole_misc.img $IMAGE_PATH/pcba_whole_misc.img
	echo "done."

if [ -d $OUT/system ]; then
  echo -n "create system.img..."
  if [ "$FSTYPE" = "cramfs" ]; then
    chmod -R 777 $OUT/system
      $FAKEROOT mkfs.cramfs $OUT/system $IMAGE_PATH/system.img
  elif [ "$FSTYPE" = "squashfs" ]; then
    chmod -R 777 $OUT/system
      mksquashfs $OUT/system $IMAGE_PATH/system.img -all-root >/dev/null
  elif [ "$FSTYPE" = "ext3" ] || [ "$FSTYPE" = "ext4" ]; then
    if [ "$BOARD_USE_SPARSE_SYSTEM_IMAGE" = "true" ]; then
      if [ $TARGET != $BOOT_OTA ]; then
        python ./build/tools/releasetools/build_image.py \
        $OUT/system $OUT/obj/PACKAGING/systemimage_intermediates/system_image_info.txt \
        $OUT/system.img $OUT/system
      fi
      python device/rockchip/common/sparse_tool.py $OUT/system.img
      mv $OUT/system.img.out $OUT/system.img
      cp -f $OUT/system.img $IMAGE_PATH/system.img
    else
      #system_size=`ls -l $OUT/system.img | awk '{print $5;}'`
      system_size=$BOARD_SYSTEMIMAGE_PARTITION_SIZE
      [ $system_size -gt "0" ] || { echo "Please make first!!!" && exit 1; }
      MAKE_EXT4FS_ARGS=" -L system -S $OUT/root/file_contexts -a system $IMAGE_PATH/system.img $OUT/system"
      ok=0
      while [ "$ok" = "0" ]; do
        make_ext4fs -l $system_size $MAKE_EXT4FS_ARGS >/dev/null 2>&1 &&
        tune2fs -c -1 -i 0 $IMAGE_PATH/system.img >/dev/null 2>&1 &&
        ok=1 || system_size=$(($system_size + 5242880))
        done
        e2fsck -fyD $IMAGE_PATH/system.img >/dev/null 2>&1 || true
    fi
  else
    mkdir -p $IMAGE_PATH/2k $IMAGE_PATH/4k
    mkyaffs2image -c 2032 -s 16 -f $OUT/system $IMAGE_PATH/2k/system.img
    mkyaffs2image -c 4080 -s 16 -f $OUT/system $IMAGE_PATH/4k/system.img
  fi
  echo "done."
fi
if [ -f $UBOOT_PATH/uboot.img ]
then
	echo -n "create uboot.img..."
	cp -a $UBOOT_PATH/uboot.img $IMAGE_PATH/uboot.img
	echo "done."
else
	echo "$UBOOT_PATH/uboot.img not fount! Please make it from $UBOOT_PATH first!"
fi

if [ -f $UBOOT_PATH/trust.img ]
then
        echo -n "create trust.img..."
        cp -a $UBOOT_PATH/trust.img $IMAGE_PATH/trust.img
        echo "done."
else    
        echo "$UBOOT_PATH/trust.img not fount! Please make it from $UBOOT_PATH first!"
fi

if [ -f $UBOOT_PATH/*_loader_*.bin ]
then
        echo -n "create loader..."
        cp -a $UBOOT_PATH/*_loader_*.bin $IMAGE_PATH/MiniLoaderAll.bin
        echo "done."
else
	if [ -f $UBOOT_PATH/*loader*.bin ]; then
		echo -n "create loader..."
		cp -a $UBOOT_PATH/*loader*.bin $IMAGE_PATH/MiniLoaderAll.bin
		echo "done."
	elif [ "$TARGET_PRODUCT" == "px3" -a -f $UBOOT_PATH/RKPX3Loader_miniall.bin ]; then
        echo -n "create loader..."
        cp -a $UBOOT_PATH/RKPX3Loader_miniall.bin $IMAGE_PATH/MiniLoaderAll.bin
        echo "done."
	else
        echo "$UBOOT_PATH/*MiniLoaderAll_*.bin not fount! Please make it from $UBOOT_PATH first!"
	fi
fi

if [ -f $KERNEL_PATH/resource.img ]
then
        echo -n "create resource.img..."
        cp -a $KERNEL_PATH/resource.img $IMAGE_PATH/resource.img
        echo "done."
else
        echo "$KERNEL_PATH/resource.img not fount!"
fi

if [ -f $KERNEL_PATH/kernel.img ]
then
        echo -n "create kernel.img..."
        cp -a $KERNEL_PATH/kernel.img $IMAGE_PATH/kernel.img
        echo "done."
else
        echo "$KERNEL_PATH/kernel.img not fount!"
fi

if [ -f $PARAMETER ]
then
        echo -n "create parameter..."
        cp -a $PARAMETER $IMAGE_PATH/parameter.txt
        echo "done."
else
        echo "$PARAMETER not fount!"
fi
chmod a+r -R $IMAGE_PATH/
