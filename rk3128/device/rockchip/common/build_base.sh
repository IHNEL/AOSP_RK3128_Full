#!/bin/bash
usage()
{
    echo "USAGE: build_base -a <ARCH> -l <LUNCH> -u <UBOOT_DEFCONFIG> -k <KERNEL_DEFCONFIG> -d <KERNEL_DTS> [-ovj]"
    echo "-a <ARCH>             -ARCH of platform(arm, arm64, x86..)"
    echo "-l <LUNCH>            -Launch Android device"
    echo "-u <UBOOT_DEFCONFIG>  -Uboot defconfig"
    echo "-k <KERNEL_DEFCONFIG> -Kernel defconfig"
    echo "-d <KERNEL_DTS>       -Kernel dts"
    echo "-o                    -Generate ota package"
    echo "-v                    -Set build version name for output image folder"
    echo "-j                    -Build jobs"
    exit 1
}

BUILD_OTA=false
BUILD_VERSION="IMAGES"
JOBS=8

# build args
ARCH=
LUNCH=
UBOOT_DEFCONFIG=
KERNEL_DEFCONFIG=
KERNEL_DTS=

# check pass argument
while getopts "ovj:l:u:k:d:a:" arg
do
    case $arg in
        o)
            echo "will build ota package"
            BUILD_OTA=true
            ;;
        v)
            BUILD_VERSION=$OPTARG
            ;;
        j)
            JOBS=$OPTARG
            ;;
        l)
            LUNCH=$OPTARG
            ;;
        u)
            UBOOT_DEFCONFIG=$OPTARG
            ;;
        k)
            KERNEL_DEFCONFIG=$OPTARG
            ;;
        d)
            KERNEL_DTS=$OPTARG
            ;;
        a)
            ARCH=$OPTARG
            ;;
        ?)
            usage ;;
    esac
done

if [[ -z $ARCH || -z $LUNCH || -z $UBOOT_DEFCONFIG || -z $KERNEL_DEFCONFIG || -z $KERNEL_DTS ]];then
    echo "Missing some mandatory args, exit!"
    echo "ARCH=$ARCH"
    echo "LUNCH=$LUNCH"
    echo "UBOOT_DEFCONFIG=$UBOOT_DEFCONFIG"
    echo "KERNEL_DEFCONFIG=$KERNEL_DEFCONFIG"
    echo "KERNEL_DTS=$KERNEL_DTS"
    exit 1
fi

source build/envsetup.sh >/dev/null && setpaths
lunch $LUNCH
TARGET_PRODUCT=`get_build_var TARGET_PRODUCT`

#set jdk version
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH
export CLASSPATH=.:$JAVA_HOME/lib:$JAVA_HOME/lib/tools.jar
# source environment and chose target product
DEVICE=`get_build_var TARGET_PRODUCT`
BUILD_VARIANT=`get_build_var TARGET_BUILD_VARIANT`
PACK_TOOL_DIR=RKTools/linux/Linux_Pack_Firmware
IMAGE_PATH=rockdev/Image-$TARGET_PRODUCT
export PROJECT_TOP=`gettop`

#lunch $DEVICE-$BUILD_VARIANT

PLATFORM_VERSION=`get_build_var PLATFORM_VERSION`
DATE=$(date  +%Y%m%d.%H%M)
STUB_PATH=Image/"$KERNEL_DTS"_"$PLATFORM_VERSION"_"$DATE"_RELEASE_TEST
STUB_PATH="$(echo $STUB_PATH | tr '[:lower:]' '[:upper:]')"
export STUB_PATH=$PROJECT_TOP/$STUB_PATH
export STUB_PATCH_PATH=$STUB_PATH/PATCHES

# build uboot
echo "start build uboot"
if [ "$ARCH" = "arm64" ];then
    ARCHV=aarch64
elif [ "$ARCH" = "arm" ];then
    ARCHV=arm
else
    echo "Unknown arch, exit!"
    exit 1
fi

cd u-boot && make ARCHV=$ARCHV distclean && make ARCHV=$ARCHV $UBOOT_DEFCONFIG && make ARCHV=$ARCHV -j$JOBS && cd -
if [ $? -eq 0 ]; then
    echo "Build uboot ok!"
else
    echo "Build uboot failed!"
    exit 1
fi

# build kernel
echo "Start build kernel"
cd kernel && make ARCH=$ARCH distclean && make ARCH=$ARCH $KERNEL_DEFCONFIG && make ARCH=$ARCH $KERNEL_DTS.img -j$JOBS && cd -
if [ $? -eq 0 ]; then
    echo "Build kernel ok!"
else
    echo "Build kernel failed!"
    exit 1
fi
# build wifi ko
#source device/rockchip/common/build_wifi_ko.sh


# build android
echo "start build android"
make installclean
make -j$JOBS
if [ $? -eq 0 ]; then
    echo "Build android ok!"
else
    echo "Build android failed!"
    exit 1
fi

# mkimage.sh
echo "make and copy android images"
./mkimage.sh
if [ $? -eq 0 ]; then
    echo "Make image ok!"
else
    echo "Make image failed!"
    exit 1
fi

if [ "$BUILD_OTA" = true ] ; then
    INTERNAL_OTA_PACKAGE_OBJ_TARGET=obj/PACKAGING/target_files_intermediates/$TARGET_PRODUCT-target_files-*.zip
    INTERNAL_OTA_PACKAGE_TARGET=$TARGET_PRODUCT-ota-*.zip
    echo "generate ota package"
    make otapackage -j$JOBS
    ./mkimage.sh ota
    cp $OUT/$INTERNAL_OTA_PACKAGE_TARGET $IMAGE_PATH/
    cp $OUT/$INTERNAL_OTA_PACKAGE_OBJ_TARGET $IMAGE_PATH/
fi


mkdir -p $PACK_TOOL_DIR/rockdev/Image/
cp -f $IMAGE_PATH/* $PACK_TOOL_DIR/rockdev/Image/

echo "Make update.img"
cd $PACK_TOOL_DIR/rockdev && ./mkupdate.sh
if [ $? -eq 0 ]; then
    echo "Make update image ok!"
else
    echo "Make update image failed!"
    exit 1
fi
cd -

mv $PACK_TOOL_DIR/rockdev/update.img $IMAGE_PATH/
rm $PACK_TOOL_DIR/rockdev/Image -rf

mkdir -p $STUB_PATH

#Generate patches
echo "cwcw$PROJECT_TOP"
.repo/repo/repo forall -c "$PROJECT_TOP/device/rockchip/common/gen_patches_body.sh"

#Copy stubs
cp manifest.xml $STUB_PATH/manifest_${DATE}.xml

mkdir -p $STUB_PATCH_PATH/kernel
cp kernel/.config $STUB_PATCH_PATH/kernel
cp kernel/vmlinux $STUB_PATCH_PATH/kernel

mkdir -p $STUB_PATH/IMAGES/
cp $IMAGE_PATH/* $STUB_PATH/IMAGES/
#Save build command info
echo "UBOOT:  defconfig: $UBOOT_DEFCONFIG" >> $STUB_PATH/build_cmd_info
echo "KERNEL: defconfig: $KERNEL_DEFCONFIG, dts: $KERNEL_DTS" >> $STUB_PATH/build_cmd_info
echo "ANDROID:$DEVICE-$BUILD_VARIANT" >> $STUB_PATH/build_cmd_info
