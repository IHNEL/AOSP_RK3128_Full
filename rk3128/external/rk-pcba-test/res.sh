#!/bin/bash
echo res.sh $1 $2 $3 $4
TARGET_PRODUCT=$1
PRODUCT_OUT=$2
TARGET_BOARD_PLATFORM=$3
TARGET_ARCH=$4
TARGET_COMMON=common
PCBA_PATH=external/rk-pcba-test
BT_BLUEDROID=true

if [ $TARGET_BOARD_PLATFORM = "rk2928" ]; then
    MODULE="modules"
else
    MODULE="modules_smp"
fi
echo MODULE $MODULE

############################################### wifi bt firmware ##################################################

mkdir -p $PRODUCT_OUT/recovery/root/system/
mkdir -p $PRODUCT_OUT/recovery/root/system/bin/
mkdir -p $PRODUCT_OUT/recovery/root/system/lib/
mkdir -p $PRODUCT_OUT/recovery/root/system/lib/modules/
mkdir -p $PRODUCT_OUT/recovery/root/system/lib/hw/
mkdir -p $PRODUCT_OUT/recovery/root/system/etc/
mkdir -p $PRODUCT_OUT/recovery/root/system/etc/firmware/
mkdir -p $PRODUCT_OUT/recovery/root/etc/firmware/
mkdir -p $PRODUCT_OUT/recovery/root/etc/bluetooth/
mkdir -p $PRODUCT_OUT/recovery/root/vendor/firmware
mkdir -p $PRODUCT_OUT/recovery/root/lib/firmware
mkdir -p $PRODUCT_OUT/recovery/root/system/etc/bluetooth
if [ -e "$PRODUCT_OUT/system/etc/firmware/" ] ; then
cp $PRODUCT_OUT/system/etc/firmware/ $PRODUCT_OUT/recovery/root/system/etc/ -a
fi
if [ -e "$PRODUCT_OUT/system/vendor/firmware/" ] ; then
cp $PRODUCT_OUT/system/vendor/firmware/ $PRODUCT_OUT/recovery/root/vendor/ -a
fi
if [ -e "$PRODUCT_OUT/system/etc/bluetooth/" ] ; then
cp $PRODUCT_OUT/system/etc/bluetooth/ $PRODUCT_OUT/recovery/root/etc/ -a
fi

if [ -e "$PRODUCT_OUT/system/etc/firmware/rtl8723b_fw" ] ; then
cp $PRODUCT_OUT/system/etc/firmware/rtl8723b_fw $PRODUCT_OUT/recovery/root/lib/firmware/rtl8723b_fw
cp $PRODUCT_OUT/system/etc/firmware/rtl8723bu_config $PRODUCT_OUT/recovery/root/lib/firmware/rtl8723bu_config
cp $PRODUCT_OUT/system/etc/firmware/rtl8723a_fw $PRODUCT_OUT/recovery/root/lib/firmware/rtl8723a_fw
cp $PRODUCT_OUT/system/etc/firmware/rtl8723a_config $PRODUCT_OUT/recovery/root/lib/firmware/rtl8723a_config
fi
############################################### ko ##################################################

if [ -e "device/rockchip/$TARGET_COMMON/ipp/lib/rk29-ipp.ko" ] ; then
cp device/rockchip/$TARGET_COMMON/ipp/lib/rk29-ipp.ko $PRODUCT_OUT/recovery/root/
fi
if [ -e "device/rockchip/$TARGET_COMMON/ipp/lib/rk29-ipp.ko.3.0.36+" ] ; then
cp device/rockchip/$TARGET_COMMON/ipp/lib/rk29-ipp.ko.3.0.36+ $PRODUCT_OUT/recovery/root/
fi

############################################### bin/lib ##################################################

if [ -e "$PRODUCT_OUT/system/bin/toolbox" ] ; then
cp $PRODUCT_OUT/system/bin/toolbox $PRODUCT_OUT/recovery/root/system/bin/
fi
if [ -e "$PRODUCT_OUT/system/bin/linker" ] ; then
cp $PRODUCT_OUT/system/bin/linker $PRODUCT_OUT/recovery/root/system/bin/
fi
if [ -e "$PRODUCT_OUT/recovery/root/sbin/sh" ] ; then
cp $PRODUCT_OUT/recovery/root/sbin/sh $PRODUCT_OUT/recovery/root/system/bin/
fi

if [ $TARGET_ARCH = "arm64" ] ; then
	if [ -e "$PRODUCT_OUT/obj_arm/lib/libselinux.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/libselinux.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj_arm/lib/libusbhost.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/libusbhost.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj_arm/lib/libc.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/libc.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj_arm/lib/libcutils.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/libcutils.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj_arm/lib/liblog.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/liblog.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj_arm/lib/libm.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/libm.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj_arm/lib/libstdc++.so" ] ; then
	cp $PRODUCT_OUT/obj_arm/lib/libstdc++.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
else
	if [ -e "$PRODUCT_OUT/obj/lib/libselinux.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/libselinux.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj/lib/libusbhost.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/libusbhost.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj/lib/libc.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/libc.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj/lib/libcutils.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/libcutils.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj/lib/liblog.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/liblog.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj/lib/libm.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/libm.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
	if [ -e "$PRODUCT_OUT/obj/lib/libstdc++.so" ] ; then
	cp $PRODUCT_OUT/obj/lib/libstdc++.so $PRODUCT_OUT/recovery/root/system/lib/
	fi
fi

if [ $BT_BLUEDROID = "true" ] ; then
	if [ $TARGET_ARCH = "arm64" ] ; then
		if [ -e "$PRODUCT_OUT/obj_arm/lib/bluetooth.default.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/bluetooth.default.so $PRODUCT_OUT/recovery/root/system/lib/hw/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/bluetooth_rtk.default.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/bluetooth_rtk.default.so $PRODUCT_OUT/recovery/root/system/lib/hw/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbluetooth_mtk.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbluetooth_mtk.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-hci.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-hci.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-utils.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-utils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/vendor/lib/libbt-vendor.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/vendor/lib/libbt-vendor.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-vendor.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-vendor.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-vendor-rtl8723bu.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-vendor-rtl8723bu.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-vendor-rtl8723bs.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-vendor-rtl8723bs.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-vendor_uart.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-vendor_uart.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbt-vendor_usb.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbt-vendor_usb.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libcorkscrew.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libcorkscrew.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libgccdemangle.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libgccdemangle.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libhardware_legacy.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libhardware_legacy.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libhardware.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libhardware.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libnetutils.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libnetutils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libpower.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libpower.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libutils.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libutils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libwpa_client.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libwpa_client.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libz.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libz.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbacktrace.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbacktrace.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libstlport.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libstlport.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libunwind.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libunwind.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libcrypto.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libcrypto.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libdl.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libdl.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libunwind-ptrace.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libunwind-ptrace.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libc++.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libc++.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libaudioutils.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libaudioutils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libinder.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libinder.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libcamera_client.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libcamera_client.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libcamera_metadata.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libcamera_metadata.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libchrome.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libchrome.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libEGL.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libEGL.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libevent.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libevent.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libexpat.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libexpat.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libGLESv.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libGLESv.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libgui.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libgui.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libicui18n.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libicui18n.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libicuuc.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libicuuc.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/liblzma.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/liblzma.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libmedia.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libmedia.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libnbaio.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libnbaio.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libpowermanager.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libpowermanager.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libprotobuf-cpp-full.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libprotobuf-cpp-full.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libsonivox.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libsonivox.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libspeexresampler.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libspeexresampler.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libstagefright_foundation.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libstagefright_foundation.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libsync.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libsync.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libui.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libui.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libbinder.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libbinder.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libGLESv2.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libGLESv2.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libnativehelper.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libnativehelper.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/lib/libnativeloader.so" ] ; then
		cp $PRODUCT_OUT/obj_arm/lib/libnativeloader.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj_arm/EXECUTABLES/bdt_intermediates/bdt" ] ; then
		cp $PRODUCT_OUT/obj_arm/EXECUTABLES/bdt_intermediates/bdt $PRODUCT_OUT/recovery/root/system/bin/
		fi
		if [ -e "$PRODUCT_OUT/system/etc/bluetooth/" ] ; then
		cp $PRODUCT_OUT/system/etc/bluetooth/ $PRODUCT_OUT/recovery/root/system/etc/ -a
		fi
	else
		if [ -e "$PRODUCT_OUT/obj/lib/bluetooth.default.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/bluetooth.default.so $PRODUCT_OUT/recovery/root/system/lib/hw/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/bluetooth_rtk.default.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/bluetooth_rtk.default.so $PRODUCT_OUT/recovery/root/system/lib/hw/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbluetooth_mtk.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbluetooth_mtk.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-hci.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-hci.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-utils.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-utils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/vendor/lib/libbt-vendor.so" ] ; then
		cp $PRODUCT_OUT/obj/vendor/lib/libbt-vendor.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-vendor.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-vendor.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-vendor-rtl8723bu.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-vendor-rtl8723bu.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-vendor-rtl8723bs.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-vendor-rtl8723bs.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-vendor_uart.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-vendor_uart.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbt-vendor_usb.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbt-vendor_usb.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libcorkscrew.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libcorkscrew.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libgccdemangle.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libgccdemangle.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libhardware_legacy.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libhardware_legacy.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libhardware.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libhardware.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libnetutils.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libnetutils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libpower.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libpower.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libutils.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libutils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libwpa_client.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libwpa_client.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libz.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libz.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbacktrace.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbacktrace.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libstlport.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libstlport.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libunwind.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libunwind.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libcrypto.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libcrypto.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libdl.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libdl.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libunwind-ptrace.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libunwind-ptrace.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libc++.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libc++.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libaudioutils.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libaudioutils.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libinder.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libinder.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libcamera_client.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libcamera_client.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libcamera_metadata.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libcamera_metadata.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libchrome.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libchrome.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libEGL.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libEGL.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libevent.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libevent.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libexpat.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libexpat.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libGLESv.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libGLESv.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libgui.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libgui.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libicui18n.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libicui18n.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libicuuc.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libicuuc.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/liblzma.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/liblzma.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libmedia.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libmedia.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libnbaio.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libnbaio.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libpowermanager.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libpowermanager.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libprotobuf-cpp-full.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libprotobuf-cpp-full.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libsonivox.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libsonivox.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libspeexresampler.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libspeexresampler.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libstagefright_foundation.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libstagefright_foundation.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libsync.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libsync.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libui.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libui.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libbinder.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libbinder.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libGLESv2.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libGLESv2.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libnativehelper.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libnativehelper.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libnativeloader.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libnativeloader.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/obj/lib/libnetd_client.so" ] ; then
		cp $PRODUCT_OUT/obj/lib/libnetd_client.so $PRODUCT_OUT/recovery/root/system/lib/
		fi
		if [ -e "$PRODUCT_OUT/system/bin/bdt" ] ; then
		cp $PRODUCT_OUT/system/bin/bdt $PRODUCT_OUT/recovery/root/system/bin/
		fi
		if [ -e "$PRODUCT_OUT/system/etc/bluetooth/" ] ; then
		cp $PRODUCT_OUT/system/etc/bluetooth/ $PRODUCT_OUT/recovery/root/system/etc/ -a
		fi
	fi
fi

# for sensor test
if [ -e "$PRODUCT_OUT/system/bin/akmd" ] ; then
cp $PRODUCT_OUT/system/bin/akmd $PRODUCT_OUT/recovery/root/system/bin/
fi
if [ -e "$PRODUCT_OUT/system/bin/sensor_test" ] ; then
cp $PRODUCT_OUT/system/bin/sensor_test $PRODUCT_OUT/recovery/root/system/bin/
fi
if [ -e "$PRODUCT_OUT/system/lib/libmllite.so" ] ; then
cp $PRODUCT_OUT/system/lib/libmllite.so $PRODUCT_OUT/recovery/root/system/lib/
fi
if [ -e "$PRODUCT_OUT/system/lib/libmplmpu.so" ] ; then
cp $PRODUCT_OUT/system/lib/libmplmpu.so $PRODUCT_OUT/recovery/root/system/lib/
fi
if [ -e "$PRODUCT_OUT/system/lib/libinvensense_hal.so" ] ; then
cp $PRODUCT_OUT/system/lib/libinvensense_hal.so $PRODUCT_OUT/recovery/root/system/lib/
fi
if [ -e "$PRODUCT_OUT/system/lib/modules/inv-mpu-iio.ko" ] ; then
mkdir -p $PRODUCT_OUT/recovery/root/system/lib/modules
cp $PRODUCT_OUT/system/lib/modules/inv-mpu-iio.ko $PRODUCT_OUT/recovery/root/system/lib/modules/
fi
if [ -e "$PRODUCT_OUT/root/drmboot.ko" ] ; then
mkdir -p $PRODUCT_OUT/recovery/root/system/lib/modules
cp $PRODUCT_OUT/root/drmboot.ko $PRODUCT_OUT/recovery/root/system/lib/modules/
fi
if [ -e "$PRODUCT_OUT/system/lib/libbase.so" ] ; then
cp $PRODUCT_OUT/system/lib/libbase.so $PRODUCT_OUT/recovery/root/system/lib/
fi
