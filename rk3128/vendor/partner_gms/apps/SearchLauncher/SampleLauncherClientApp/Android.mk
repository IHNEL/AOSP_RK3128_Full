LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := SampleLauncherClientApp
LOCAL_SDK_VERSION := current
LOCAL_MIN_SDK_VERSION := 23

LOCAL_STATIC_JAVA_LIBRARIES := libLauncherClient

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

include $(BUILD_PACKAGE)
