LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	auth.c \
	ccp.c \
	chap-md5.c \
	chap-new.c \
	chap_ms.c \
	demand.c \
	eap.c \
	ecp.c \
	eui64.c \
	fsm.c \
	ipcp.c \
	ipv6cp.c \
	lcp.c \
	magic.c \
	main.c \
	options.c \
	pppcrypt.c \
	pppox.c \
	session.c \
	sys-linux.c \
	tty.c \
	upap.c \
	utils.c

# options.c:623:21: error: passing 'const char *' to parameter of type 'char *' discards qualifiers.
# [-Werror,-Wincompatible-pointer-types-discards-qualifiers]
LOCAL_CLANG_CFLAGS += -Wno-incompatible-pointer-types-discards-qualifiers

LOCAL_SHARED_LIBRARIES := \
	libcutils liblog libcrypto libdl

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
        system/core/include/netutils \
        $(LOCAL_PATH)/plugins/rp-pppoe \
        $(LOCAL_PATH)  \
        external/ppp/  \
        $(LOCAL_PATH)/include/net

LOCAL_CFLAGS := -DCHAPMS=1 -DMPPE=1 -DINET6=1 -DUSE_OPENSSL=1 -Wno-unused-parameter -Wno-empty-body -Wno-missing-field-initializers -Wno-attributes -Wno-sign-compare -Wno-pointer-sign -Werror -Wno-unused-const-variable

# Turn off warnings for now until this is fixed upstream. b/18632512
LOCAL_CFLAGS += -Wno-unused-variable

LOCAL_CFLAGS += -DPLUGIN -DMAXOCTETS -O2 -pipe -Wall -g -Wno-unused
LOCAL_CFLAGS += -DKERNEL_MODE_PPPOE -DDEBUGGING_ENABLED

LOCAL_LDLIBS += -ldl
LOCAL_LDFLAGS :=-Wl,-E
LOCAL_MULTILIB := 32

LOCAL_MODULE:= pppd

include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)
#
#LOCAL_SRC_FILES:= \
       plugins/rp-pppoe/discovery.c \
       plugins/rp-pppoe/common.c \
       plugins/rp-pppoe/pppoe-discovery.c \
       plugins/rp-pppoe/debug.c \
       plugins/rp-pppoe/if.c 
# 
#LOCAL_SHARED_LIBRARIES := \
        libcutils liblog
#
#LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include \
        system/core/include/netutils \
        $(LOCAL_PATH)/plugins/rp-pppoe \
        $(LOCAL_PATH)  \
        external/ppp/  \
        $(LOCAL_PATH)/include/net
#LOCAL_CFLAGS := -DANDROID_CHANGES -DCHAPMS=1 -DMPPE=1 \
        -g '-DRP_VERSION="3.3"' -Wno-implicit-function-declaration
#LOCAL_CFLAGS +=-DKERNEL_MODE_PPPOE -DDEBUGGING_ENABLED
#
#LOCAL_MODULE:= pppoe-discovery
#
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        plugins/rp-pppoe/common.c \
        plugins/rp-pppoe/debug.c \
        plugins/rp-pppoe/if.c \
        plugins/rp-pppoe/plugin.c \
	plugins/rp-pppoe/discovery.c
#	lcp.c

LOCAL_SHARED_LIBRARIES := \
        libcutils liblog libcrypto
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include \
        system/core/include/netutils \
        $(LOCAL_PATH)/plugins/rp-pppoe \
        $(LOCAL_PATH)  \
        external/ppp/  \
        $(LOCAL_PATH)/include/net

LOCAL_CFLAGS :=-DANDROID_CHANGES '-DRP_VERSION="3.3"' -Wno-implicit-function-declaration
LOCAL_CFLAGS +=-DKERNEL_MODE_PPPOE -DDEBUGGING_ENABLED -c -O2 -g -shared -fPIC -Wno-unused
#LOCAL_CFLAGS :=-DANDROID_CHANGES -DCHAPMS=1 -DMPPE=1 \
               -c -O2 -g -shared -fPIC '-DVERSION="3.3"' '-DRP_VERSION="3.3"'

LOCAL_MODULE:=librp-pppoe

include $(BUILD_SHARED_LIBRARY)
