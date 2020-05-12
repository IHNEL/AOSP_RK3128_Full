BUILD_DISPLAYD := false
ifneq ($(TARGET_SIMULATOR),true)
    BUILD_DISPLAYD := true
endif

ifeq ($(BUILD_DISPLAYD),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=                       \
                  main.cpp              \
                  DisplayManager.cpp	\
                  CommandListener.cpp	\
                  NetlinkManager.cpp    \
                  NetlinkHandler.cpp    \
                  DisplaydCommand.cpp   \
                  OtgManager.cpp		\
                  ScreenScaleManager.cpp\
                  Hdcp.cpp \
                  BcshManager.cpp \
                  Cecmanager.cpp


LOCAL_MODULE:= displayd
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
                    external/openssl/include
LOCAL_CFLAGS :=

ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)), box)
	LOCAL_CFLAGS += -DDISPLAY_POLICY_BOX
endif
ifneq ($(filter rk3228 rk3328 rk322x, $(strip $(TARGET_BOARD_PLATFORM))), )
	LOCAL_CFLAGS += -DRK3228
endif
ifeq ($(strip $(PLATFORM_VERSION)),4.4.4)
	LOCAL_CFLAGS += -DANDROID_4_4
endif

ifeq (1,$(strip $(shell expr $(PLATFORM_SDK_VERSION) \>= 23)))
	LOCAL_CFLAGS += -DANDROID_6
endif

LOCAL_SHARED_LIBRARIES := libcutils libnetutils libcrypto libsysutils

ifeq ($(PRODUCT_HAVE_HDMIHDCP2), true)
	LOCAL_CFLAGS += -DSUPPORT_HDCP2
	LOCAL_SHARED_LIBRARIES += librkhdcp2
endif

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_SRC_FILES:=		\
                  ddc.c \

LOCAL_MODULE:= ddc
LOCAL_MODULE_TAGS := optional
#LOCAL_C_INCLUDES := $(KERNEL_HEADERS)

LOCAL_CFLAGS := 

LOCAL_SHARED_LIBRARIES := libcutils

include $(BUILD_EXECUTABLE)

endif # ifeq ($(BUILD_HDMID,true)
