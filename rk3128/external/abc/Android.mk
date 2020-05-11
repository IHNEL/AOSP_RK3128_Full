LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	abc.c \
	misc.c \
	mail.c

LOCAL_MODULE := abc
LOCAL_MODULE_TAGS := eng
LOCAL_SHARED_LIBRARIES := liblog
include $(BUILD_EXECUTABLE)
