LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH), arm64)
  LOCAL_PREBUILT_LIBS := lib64/librkupdate.a
else ifeq ($(TARGET_ARCH), arm)
  LOCAL_PREBUILT_LIBS := lib/librkupdate.a
endif

include $(BUILD_MULTI_PREBUILT)
