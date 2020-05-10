LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    events.cpp \
    graphics.cpp \
    graphics_adf.cpp \
    graphics_drm.cpp \
    graphics_fbdev.cpp \
    resources.cpp \

LOCAL_WHOLE_STATIC_LIBRARIES += libadf
LOCAL_WHOLE_STATIC_LIBRARIES += libdrm
LOCAL_STATIC_LIBRARIES += libpng

LOCAL_MODULE := libminui

LOCAL_CLANG := true

# This used to compare against values in double-quotes (which are just
# ordinary characters in this context).  Strip double-quotes from the
# value so that either will work.

ifeq ($(subst ",,$(TARGET_RECOVERY_PIXEL_FORMAT)),ABGR_8888)
  LOCAL_CFLAGS += -DRECOVERY_ABGR
endif
ifeq ($(subst ",,$(TARGET_RECOVERY_PIXEL_FORMAT)),RGBX_8888)
  LOCAL_CFLAGS += -DRECOVERY_RGBX
endif
ifeq ($(subst ",,$(TARGET_RECOVERY_PIXEL_FORMAT)),BGRA_8888)
  LOCAL_CFLAGS += -DRECOVERY_BGRA
endif

#rotate screen to 0, 90, 180, 270
#0:   rotate_0
#90:  rotate_90
#180: rotate_180
#270: rotate_270
ifeq ($(strip $(ROTATE_SCREEN)), rotate_0)
  LOCAL_CFLAGS += -DRotateScreen_0
endif
ifeq ($(strip $(ROTATE_SCREEN)), rotate_90)
  LOCAL_CFLAGS += -DRotateScreen_90
endif
ifeq ($(strip $(ROTATE_SCREEN)), rotate_180)
  LOCAL_CFLAGS += -DRotateScreen_180
endif
ifeq ($(strip $(ROTATE_SCREEN)), rotate_270)
  LOCAL_CFLAGS += -DRotateScreen_270
endif

ifneq ($(TARGET_RECOVERY_OVERSCAN_PERCENT),)
  LOCAL_CFLAGS += -DOVERSCAN_PERCENT=$(TARGET_RECOVERY_OVERSCAN_PERCENT)
else
  LOCAL_CFLAGS += -DOVERSCAN_PERCENT=0
endif

ifeq ($(TARGET_BOARD_PLATFORM_PRODUCT),box)
  LOCAL_CFLAGS += -DPLATFORM_PRODUCT_BOX
endif

include $(BUILD_STATIC_LIBRARY)

# Used by OEMs for factory test images.
include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_MODULE := libminui
LOCAL_WHOLE_STATIC_LIBRARIES += libminui
LOCAL_SHARED_LIBRARIES := libpng
include $(BUILD_SHARED_LIBRARY)
