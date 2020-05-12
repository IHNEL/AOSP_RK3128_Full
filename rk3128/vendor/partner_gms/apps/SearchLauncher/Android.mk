LOCAL_PATH := $(call my-dir)

#
# Prebuilt Launcher client library Libraries
#
include $(CLEAR_VARS)
LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := \
    libLauncherClient:libs/launcher_client.jar
include $(BUILD_MULTI_PREBUILT)

#
# Search launcher build rule
#
include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := SearchLauncher
LOCAL_SDK_VERSION := current
LOCAL_MIN_SDK_VERSION := 21
LOCAL_OVERRIDES_PACKAGES := Home Launcher2 Launcher3
LOCAL_PRIVILEGED_MODULE := true

# Relative path for Launcher3 directory
LAUNCHER_PATH := ../../../../packages/apps/Launcher3

LOCAL_STATIC_JAVA_LIBRARIES := \
    android-support-v4 \
    android-support-v7-recyclerview \
    android-support-v7-palette \
    libLauncherClient

LOCAL_SRC_FILES := \
    $(call all-java-files-under, src) \
    $(call all-java-files-under, $(LAUNCHER_PATH)/src) \
    $(call all-java-files-under, $(LAUNCHER_PATH)/src_config) \
    $(call all-proto-files-under, $(LAUNCHER_PATH)/protos)

LOCAL_RESOURCE_DIR := \
    $(LOCAL_PATH)/res \
    $(LOCAL_PATH)/$(LAUNCHER_PATH)/res \
    frameworks/support/v7/recyclerview/res

LOCAL_FULL_LIBS_MANIFEST_FILES := \
    $(LOCAL_PATH)/$(LAUNCHER_PATH)/AndroidManifest-common.xml

LOCAL_PROGUARD_FLAG_FILES := $(LAUNCHER_PATH)/proguard.flags

LOCAL_PROTOC_OPTIMIZE_TYPE := nano
LOCAL_PROTOC_FLAGS := --proto_path=$(LOCAL_PATH)/$(LAUNCHER_PATH)/protos/
LOCAL_AAPT_FLAGS := \
    --auto-add-overlay \
    --extra-packages android.support.v7.recyclerview \
    --extra-packages com.android.launcher3

include $(BUILD_PACKAGE)

# ==================================================
include $(call all-makefiles-under,$(LOCAL_PATH))
