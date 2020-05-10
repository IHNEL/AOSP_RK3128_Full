#!/bin/bash

. build/envsetup.sh >/dev/null && setpaths

export PATH=$ANDROID_BUILD_PATHS:$PATH

TARGET_PRODUCT=`get_build_var TARGET_PRODUCT`

TARGET_ARCH=`get_build_var TARGET_ARCH`

ANDROID_BUILD_TOP=`get_build_var ANDROID_BUILD_TOP`

echo ANDROID_BUILD_TOP=$ANDROID_BUILD_TOP

echo TARGET_PRODUCT=$TARGET_PRODUCT

if [  "$TARGET_PRODUCT" == "rk3228h" ];then
        TARGET_ARCH="arm64"
fi
echo TARGET_ARCH = $TARGET_ARCH
echo "---- make wifi ko  ----"

make -C kernel ARCH=$TARGET_ARCH  modules -j18
find $ANDROID_BUILD_TOP/kernel/drivers/net/wireless/rockchip_wlan/*  -name "*.ko" | \
xargs -n1 -i cp {} $ANDROID_BUILD_TOP/vendor/rockchip/common/wifi/modules
