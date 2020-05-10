/*
 * emmcutils.c
 *
 *  Created on: 2013-7-30
 *      Author: mmk@rock-chips.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <fs_mgr.h>
#include "rk_emmcutils.h"
#include "cutils/properties.h"

typedef struct fstab_rec Volume;
static struct fstab *fstab = NULL;
int getEmmcState() {
    char bootmode[256];
    int result = 0;

    property_get("ro.bootmode", bootmode, "unknown");
    printf("bootmode = %s \n", bootmode);

    if(!strcmp(bootmode, "nvme")) {
	result = 2;
    } else if(!strcmp(bootmode, "emmc")) {
        result = 1;
    }else {
        result = 0;
    }

    return result;
}



static void load_volume_table()
{
    int ret;
    char *name_fstab = "/fstab.rk30board";

    if(fstab == NULL) {
        fstab = fs_mgr_read_fstab(name_fstab);
        if (!fstab) {
            printf("failed to read android.fstab\n");
            return ;
        }
    }

    ret = fs_mgr_add_entry(fstab, "/tmp", "ramdisk", "ramdisk");
    if (ret < 0 ) {
        printf("failed to add /tmp entry to fstab\n");
        fs_mgr_free_fstab(fstab);
        fstab = NULL;
        return;
    }
}

static Volume* volume_for_path(const char* path) {
    return fs_mgr_get_entry_for_mount_point(fstab, path);
}

char* getDevicePath(char *mtdDevice) {
    int emmcEnabled = getEmmcState();
    char devicePath[128] = "/";
    if(emmcEnabled) {
        printf("must free fstab, before use it.\n");
        fs_mgr_free_fstab(fstab);
        fstab = NULL;
        if(fstab == NULL) {
            load_volume_table();
            if(fstab == NULL){
                printf("getDevicePath: Cannot read fstab.\n");
                return mtdDevice; 
            }
        }
        if(strstr(mtdDevice, "/dev/block/rknand_")) {
            if(strcmp(mtdDevice+18, "userdata") == 0){
                strcat(devicePath, "data");
            }else{
                strcat(devicePath, mtdDevice+18);
            }
            printf("mtd device %s\n", devicePath);
            Volume* v = volume_for_path(devicePath);
            if (v != NULL) {
                printf("getDevicePath: get volume path %s\n", v->blk_device);
                return v->blk_device;
            }else {
                printf("getDevicePath: Cannot load volume %s!\n", devicePath);
            }
        }
    }
    return mtdDevice;
}
static char deviceName[128] = "\0";
int transformPath(const char *in, char *out) {
    if(strlen(deviceName) == 0){
        if(fstab == NULL)
        load_volume_table();
        if(in == NULL || out == NULL) {
            printf("transformPath argument can't be NULL\n");
            return -1;
        }
        printf("transformPath in: %s\n", in);
        Volume* v = volume_for_path("/system");
        if(v !=  NULL){
            strcpy(deviceName, v->blk_device);
        }else{
            printf("Cannot load volume %s!\n", "/system");
            return -1;
        }
    }
    if (strlen(deviceName) != 0) {
        printf("get volume path %s\n", deviceName);
        int len = strlen(deviceName);
        strncpy(out, deviceName, len-6);
        *(out + len - 6) = '\0';
    }else {
        printf("Cannot load volume %s!\n", "/system");
    }
    printf("\n");

    //fs_mgr_free_fstab(fstab);
    strcat(out, in);
    printf("transformPath out: %s\n", out);

    return 0;
}
