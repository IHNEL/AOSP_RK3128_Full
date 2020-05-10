#
# Copyright 2014 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

PRODUCT_PACKAGES += \
    memtrack.$(TARGET_BOARD_PLATFORM) \

PRODUCT_PACKAGES += \
    libjbigi

#enable this for support f2fs with data partion
#BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := f2fs
# This ensures the needed build tools are available.
# TODO: make non-linux builds happy with external/f2fs-tool; system/extras/f2fs_utils
#ifeq ($(HOST_OS),linux)
#TARGET_USERIMAGES_USE_F2FS := true
#endif

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.${TARGET_BOARD_PLATFORM_PRODUCT}.rc:root/init.${TARGET_BOARD_PLATFORM_PRODUCT}.rc \
    $(LOCAL_PATH)/wake_lock_filter.xml:system/etc/wake_lock_filter.xml \
    $(LOCAL_PATH)/fstab.rk30board.bootmode.unknown:root/fstab.rk30board.bootmode.unknown \
    $(LOCAL_PATH)/fstab.rk30board.bootmode.emmc:root/fstab.rk30board.bootmode.emmc

ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)), box)
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rk3128_box/audio_policy_rk3128.conf:system/etc/audio_policy.conf
endif

#PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,$(LOCAL_PATH)/Sounds/Sleep,sdcard/Sounds/Sleep)

PRODUCT_COPY_FILES += \
        device/rockchip/rk312x/device_owner_2.xml:system/etc/device_owner_2.xml

PRODUCT_COPY_FILES += \
        device/rockchip/rk312x/package_performance.xml:system/etc/package_performance.xml
# setup dalvik vm configs.
$(call inherit-product, frameworks/native/build/tablet-7in-hdpi-1024-dalvik-heap.mk)
$(call inherit-product-if-exists, vendor/rockchip/rk3126/device-vendor.mk)

# add for Rogue 
PRODUCT_PACKAGES += libdrm
#for enable optee support
ifeq ($(strip $(PRODUCT_HAVE_OPTEE)),true)

PRODUCT_PROPERTY_OVERRIDES += \
       ro.enable.optee=true

ifeq ($(strip $(PRODUCT_SYSTEM_VERITY)),true)
PRODUCT_COPY_FILES += \
       device/rockchip/common/init.optee_verify.rc:root/init.optee.rc
else
PRODUCT_COPY_FILES += \
       device/rockchip/common/init.optee.rc:root/init.optee.rc
endif
endif
