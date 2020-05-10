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
# This file is the build configuration for an aosp Android
# build for rockchip rk3126 hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps). Except for a few implementation
# details, it only fundamentally contains two inherit-product
# lines, aosp and rk3126, hence its name.

include device/rockchip/rk312x/BoardConfig.mk
# Inherit from those products. Most specific first.
$(call inherit-product, device/rockchip/rk312x/product.mk)
$(call inherit-product, device/rockchip/common/device.mk)
$(call inherit-product, device/rockchip/common/tv/tv_base.mk)

PRODUCT_CHARACTERISTICS := box 

PRODUCT_NAME := rk3128_box
PRODUCT_DEVICE := rk3128_box
PRODUCT_BRAND := Android
PRODUCT_MODEL := rk3128_box
PRODUCT_MANUFACTURER := rockchip

# Get the long list of APNs
PRODUCT_COPY_FILES += vendor/rockchip/common/phone/etc/apns-full-conf.xml:system/etc/apns-conf.xml
PRODUCT_COPY_FILES += vendor/rockchip/common/phone/etc/spn-conf.xml:system/etc/spn-conf.xml

PRODUCT_COPY_FILES += \
	device/rockchip/rk312x/rk3128_box/20050030_pwm.kl:system/usr/keylayout/20050030_pwm.kl \

PRODUCT_AAPT_CONFIG := normal tvdpi hdpi
PRODUCT_AAPT_PREF_CONFIG := tvdpi
