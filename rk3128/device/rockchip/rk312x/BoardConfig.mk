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

# Use the non-open-source parts, if they're present

-include device/rockchip/common/BoardConfig.mk

TARGET_PREBUILT_KERNEL := kernel/arch/arm/boot/zImage

TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon

TARGET_CPU_VARIANT := cortex-a7

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true

ENABLE_CPUSETS := true

BOARD_WITH_MEM_OPTIMISE := true
BOARD_HAVE_BLUETOOTH ?= true
BOARD_HAVE_FLASH := false
BOARD_SUPPORT_HDMI ?= false
PRODUCT_PACKAGE_OVERLAYS += device/rockchip/rk312x/overlay

ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)), box)
	PRODUCT_HAVE_OPTEE ?= false
else
	PRODUCT_HAVE_OPTEE ?= false
endif

TARGET_BOARD_PLATFORM ?= rk312x
TARGET_BOARD_PLATFORM_GPU := mali400

# Sensors
BOARD_SENSOR_ST := false
BOARD_GRAVITY_SENSOR_SUPPORT := false
#BOARD_SENSOR_MPU := true
#BOARD_USES_GENERIC_INVENSENSE := false

#MALLOC_IMPL := dlmalloc
MALLOC_SVELTE := true

BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/rockchip/rk312x/bluetooth

# Copy RK3328 own init.rc file
GRAPHIC_MEMORY_PROVIDER := dma_buf
#BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2013265920
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2147483648

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(HOST_OS),linux)
    ifeq ($(TARGET_BUILD_VARIANT),user)
  	WITH_DEXPREOPT := true
    else
	WITH_DEXPREOPT := false
    endif
endif

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay
PRODUCT_AAPT_CONFIG := normal ldpi mdpi large xlarge hdpi xhdpi
