LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := rk_emmcutils.cpp

LOCAL_MODULE := librk_emmcutils
LOCAL_STATIC_LIBRARIES = libfs_mgr libcutils

include $(BUILD_STATIC_LIBRARY)
