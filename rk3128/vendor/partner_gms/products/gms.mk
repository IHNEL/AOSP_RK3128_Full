###############################################################################
# GMS home folder location
# Note: we cannot use $(my-dir) in this makefile
ANDROID_PARTNER_GMS_HOME := vendor/partner_gms

# GMS mandatory core packages
PRODUCT_PACKAGES := \
    ConfigUpdater \
    GoogleBackupTransport \
    GoogleExtServices \
    GoogleExtShared \
    GoogleFeedback \
    GoogleLoginService \
    GoogleOneTimeInitializer \
    GooglePackageInstaller \
    GooglePartnerSetup \
    GooglePrintRecommendationService \
    GoogleServicesFramework \
    GoogleCalendarSyncAdapter \
    GoogleContactsSyncAdapter \
    GoogleTTS \
    GmsCore \
    Phonesky \
    SetupWizard \
    talkback \
    WebViewGoogle

# GMS mandatory libraries
PRODUCT_PACKAGES += \
    com.google.android.maps.jar \
    com.google.android.media.effects.jar

# Overlay For GMS devices
$(call inherit-product, device/sample/products/backup_overlay.mk)
$(call inherit-product, device/sample/products/location_overlay.mk)
PRODUCT_PACKAGE_OVERLAYS += $(ANDROID_PARTNER_GMS_HOME)/products/gms_overlay

# Configuration files for GMS apps
PRODUCT_COPY_FILES += \
    $(ANDROID_PARTNER_GMS_HOME)/etc/preferred-apps/google.xml:system/etc/preferred-apps/google.xml \
    $(ANDROID_PARTNER_GMS_HOME)/etc/sysconfig/google.xml:system/etc/sysconfig/google.xml

# GMS mandatory application packages
# Note: Duo is mandatory for telephony devices, whereas Hangouts is for non-telephony devices.
PRODUCT_PACKAGES += \
    Chrome \
    Drive \
    Gmail2 \
    Hangouts \
    Maps \
    Music2 \
    Photos \
    Velvet \
    Videos \
    YouTube \
    CalendarGoogle
# GMS sample application packages
#PRODUCT_PACKAGES += \
#    GmsSampleLayout
# Overrides
PRODUCT_PROPERTY_OVERRIDES += \
    ro.setupwizard.mode=OPTIONAL \
    ro.com.google.gmsversion=7.1_r4
