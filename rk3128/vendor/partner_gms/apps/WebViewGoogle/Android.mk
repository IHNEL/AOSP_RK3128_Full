###############################################################################
# WebView Chromium
LOCAL_PATH := $(call my-dir)

my_archs := arm arm64 x86 x86_64
my_src_arch := $(call get-prebuilt-src-arch, $(my_archs))

# make sure to include correct WebView binary: full or stub
_list_of_old_api_levels := 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
ifneq (,$(filter $(PLATFORM_SDK_VERSION),$(_list_of_old_api_levels)))
  # WebView apks in this folder are for N or later platforms only, stop building
  $(error PLATFORM_SDK_VERSION must be equal to or greater than 24)
endif

_ro_property_string := $(filter ro.product.first_api_level=%,$(ADDITIONAL_BUILD_PROPERTIES))

ifdef _ro_property_string
  _first_api_level := $(word 2,$(subst =,$(space),$(_ro_property_string)))
else
  # if the first_api_level property is undefined, stop building
  $(error ro.product.first_api_level property is undefined)
endif

# preload WebView full apk by default
_gms_preload_webview_type := full

ifneq ($(filter Chrome,$(ALL_MODULES)),)
  # Chrome browser is included in this build
  ifeq (,$(filter $(_first_api_level),$(_list_of_old_api_levels)))
    # first_api_level is equal to or greater than 24
    _gms_preload_webview_type := stub
  endif
endif

include $(CLEAR_VARS)
LOCAL_MODULE := WebViewGoogle
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := optional
LOCAL_BUILT_MODULE_STEM := package.apk
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
#LOCAL_PRIVILEGED_MODULE :=
LOCAL_CERTIFICATE := PRESIGNED
LOCAL_OVERRIDES_PACKAGES := webview
$(info preloading WebView $(_gms_preload_webview_type) apk.)
ifeq "$(_gms_preload_webview_type)" "full"
  LOCAL_SRC_FILES := $(LOCAL_MODULE)_$(my_src_arch).apk
else ifeq "$(_gms_preload_webview_type)" "stub"
  LOCAL_SRC_FILES := WebViewStub.apk
endif
LOCAL_MULTILIB := both
LOCAL_REQUIRED_MODULES := \
    libwebviewchromium_loader \
    libwebviewchromium_plat_support
include $(BUILD_PREBUILT)
