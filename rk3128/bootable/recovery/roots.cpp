/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#include <fs_mgr.h>
#include "mtdutils/mtdutils.h"
#include "mtdutils/mounts.h"
#include "roots.h"
#include "common.h"
#include "make_ext4fs.h"
#include "wipe.h"
#include "cryptfs.h"
#include "emmcutils/rk_emmcutils.h"
#include "rktools.h"


static struct fstab *fstab = NULL;

extern struct selabel_handle *sehandle;

void load_volume_table()
{
    int i;
    int ret;

    int emmcState = getEmmcState();
    if (emmcState == 2) {
	fstab = fs_mgr_read_fstab("/etc/recovery.nvme.fstab");
    } else if(emmcState == 1) {
        fstab = fs_mgr_read_fstab("/etc/recovery.emmc.fstab");
    }else {
        fstab = fs_mgr_read_fstab("/etc/recovery.fstab");
    }

    if (!fstab) {
        LOGE("failed to read /etc/recovery.fstab\n");
        return;
    }

    ret = fs_mgr_add_entry(fstab, "/tmp", "ramdisk", "ramdisk");
    if (ret < 0 ) {
        LOGE("failed to add /tmp entry to fstab\n");
        fs_mgr_free_fstab(fstab);
        fstab = NULL;
        return;
    }

    printf("recovery filesystem table\n");
    printf("=========================\n");
    for (i = 0; i < fstab->num_entries; ++i) {
        Volume* v = &fstab->recs[i];
        printf("  %d %s %s %s %lld\n", i, v->mount_point, v->fs_type,
               v->blk_device, v->length);
    }
    printf("\n");
}

Volume* volume_for_path(const char* path) {
    return fs_mgr_get_entry_for_mount_point(fstab, path);
}

// Mount the volume specified by path at the given mount_point.
int ensure_path_mounted_at(const char* path, const char* mount_point) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        LOGE("unknown volume for path [%s]\n", path);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted.
        return 0;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return -1;
    }

    if (!mount_point) {
        mount_point = v->mount_point;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(mount_point);
    if (mv) {
        // volume is already mounted
        return 0;
    }

    mkdir(mount_point, 0755);  // in case it doesn't already exist

    if (strcmp(v->fs_type, "yaffs2") == 0) {
        // mount an MTD partition as a YAFFS2 filesystem.
        mtd_scan_partitions();
        const MtdPartition* partition;
        partition = mtd_find_partition_by_name(v->blk_device);
        if (partition == NULL) {
            LOGE("failed to find \"%s\" partition to mount at \"%s\"\n",
                 v->blk_device, mount_point);
            return -1;
        }
        return mtd_mount_partition(partition, mount_point, v->fs_type, 0);
    } else if (strcmp(v->fs_type, "ext4") == 0 ||
    		   strcmp(v->fs_type, "f2fs") == 0 ||
               strcmp(v->fs_type, "squashfs") == 0) {
        result = mount(v->blk_device, mount_point, v->fs_type,
                       v->flags, v->fs_options);
        if (result == 0) return 0;

        LOGE("failed to mount %s (%s)\n", mount_point, strerror(errno));
        return -1;
    }else if(strcmp(v->fs_type, "vfat") == 0){
        char *blk_device;
        blk_device = v->blk_device;
        if(strcmp("/mnt/external_sd", v->mount_point) == 0){
            blk_device = getenv(SD_POINT_NAME);
            if(blk_device == NULL){
                setFlashPoint();
                blk_device = getenv(SD_POINT_NAME);
            }
        }
        result = mount(blk_device, v->mount_point, v->fs_type,
                       MS_NOATIME | MS_NODEV | MS_NODIRATIME, "shortname=mixed,utf8");
        if (result == 0) return 0;

        LOGW("trying mount %s to ntfs\n", blk_device);
        result = mount(blk_device, v->mount_point, "ntfs",
                       MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
        if (result == 0) return 0;

        char *sec_dev = v->fs_options;
        if(strcmp("/mnt/external_sd", v->mount_point) == 0){
            sec_dev = getenv(SD_POINT_NAME_2);
        }
        if(sec_dev != NULL) {
            char *temp = strchr(sec_dev, ',');
            if(temp) {
                temp[0] = '\0';
            }

            result = mount(sec_dev, v->mount_point, v->fs_type,
                           MS_NOATIME | MS_NODEV | MS_NODIRATIME, "shortname=mixed,utf8");
            if (result == 0) return 0;

            LOGW("trying mount %s to ntfs\n", sec_dev);
            result = mount(sec_dev, v->mount_point, "ntfs",
                           MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
            if (result == 0) return 0;
        }
        LOGE("failed to mount %s (%s)\n", v->mount_point, strerror(errno));
        return -1;
    }

    LOGE("unknown fs_type \"%s\" for %s\n", v->fs_type, mount_point);
    return -1;
}

int ensure_path_mounted(const char* path) {
    if(strncmp(path, "/mnt/usb_storage", 16) == 0) {
        printf("the path is already mounted!\n");
        return 0;
    }
    // Mount at the default mount point.
    return ensure_path_mounted_at(path, nullptr);
}

int ensure_path_unmounted(const char* path) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        LOGE("unknown volume for path [%s]\n", path);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted; you can't unmount it.
        return -1;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return -1;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(v->mount_point);
    if (mv == NULL) {
        // volume is already unmounted
        return 0;
    }

    return unmount_mounted_volume(mv);
}

static int exec_cmd(const char* path, char* const argv[]) {
    int status;
    pid_t child;
    if ((child = vfork()) == 0) {
        execv(path, argv);
        _exit(-1);
    }
    waitpid(child, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        LOGE("%s failed with status %d\n", path, WEXITSTATUS(status));
    }
    return WEXITSTATUS(status);
}

int run(const char *filename, char *const argv[])
{
    struct stat s;
    int status;
    pid_t pid;

    if (stat(filename, &s) != 0) {
        fprintf(stderr, "cannot find '%s'", filename);
        return -1;
    }

    printf("executing '%s'\n", filename);

    pid = fork();

    if (pid == 0) {
        setpgid(0, getpid());
        /* execute */
        execv(filename, argv);
        fprintf(stderr, "can't run %s (%s)\n", filename, strerror(errno));
        /* exit */
        _exit(0);
    }

    if (pid < 0) {
        fprintf(stderr, "failed to fork and start '%s'\n", filename);
        return -1;
    }

    if (-1 == waitpid(pid, &status, WCONTINUED | WUNTRACED)) {
        fprintf(stderr, "wait for child error\n");
        return -1;
    }

    if (WIFEXITED(status)) {
        printf("executed '%s' done\n", filename);
    }

    printf("executed '%s' return %d\n", filename, WEXITSTATUS(status));
    return 0;
}

int format_volume(const char* volume, const char* directory) {


    Volume* v = volume_for_path(volume);
    if (v == NULL) {
        LOGE("unknown volume \"%s\"\n", volume);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // you can't format the ramdisk.
        LOGE("can't format_volume \"%s\"", volume);
        return -1;
    }
    if (strcmp(v->mount_point, volume) != 0) {
        LOGE("can't give path \"%s\" to format_volume\n", volume);
        return -1;
    }

    if (ensure_path_unmounted(volume) != 0) {
        LOGE("format_volume failed to unmount \"%s\"\n", v->mount_point);
        return -1;
    }

    if (strcmp(v->fs_type, "yaffs2") == 0 || strcmp(v->fs_type, "mtd") == 0) {
        mtd_scan_partitions();
        const MtdPartition* partition = mtd_find_partition_by_name(v->blk_device);
        if (partition == NULL) {
            LOGE("format_volume: no MTD partition \"%s\"\n", v->blk_device);
            return -1;
        }

        MtdWriteContext *write = mtd_write_partition(partition);
        if (write == NULL) {
            LOGW("format_volume: can't open MTD \"%s\"\n", v->blk_device);
            return -1;
        } else if (mtd_erase_blocks(write, -1) == (off_t) -1) {
            LOGW("format_volume: can't erase MTD \"%s\"\n", v->blk_device);
            mtd_write_close(write);
            return -1;
        } else if (mtd_write_close(write)) {
            LOGW("format_volume: can't close MTD \"%s\"\n", v->blk_device);
            return -1;
        }
        return 0;
    }

    if (strcmp(v->fs_type, "ext4") == 0 || strcmp(v->fs_type, "f2fs") == 0) {
        // if there's a key_loc that looks like a path, it should be a
        // block device for storing encryption metadata.  wipe it too.
        if (v->key_loc != NULL && v->key_loc[0] == '/') {
            LOGI("wiping %s\n", v->key_loc);
            int fd = open(v->key_loc, O_WRONLY | O_CREAT, 0644);
            if (fd < 0) {
                LOGE("format_volume: failed to open %s\n", v->key_loc);
                return -1;
            }
            wipe_block_device(fd, get_file_size(fd));
            close(fd);
        }

        ssize_t length = 0;
        if (v->length != 0) {
            length = v->length;
        } else if (v->key_loc != NULL && strcmp(v->key_loc, "footer") == 0) {
            length = -CRYPT_FOOTER_OFFSET;
        }
        int result;
        if (strcmp(v->fs_type, "ext4") == 0) {
            result = make_ext4fs_directory(v->blk_device, length, volume, sehandle, directory);
            /* check the ext4 filesystem */
            if (strcmp(volume, "/data") == 0) {
                const char *const e2fsck_argv[] = { "/sbin/e2fsck", "-fy", v->blk_device, NULL };
                printf("e2fsck check '%s' filesystem\n", v->blk_device);
                result = run(e2fsck_argv[0], (char **) e2fsck_argv);
                if(result) {
                    printf("e2fsck check '%s' fail!\n", v->blk_device);
                    return result;
                }
            }
        } else {   /* Has to be f2fs because we checked earlier. */
            if (v->key_loc != NULL && strcmp(v->key_loc, "footer") == 0 && length < 0) {
                LOGE("format_volume: crypt footer + negative length (%zd) not supported on %s\n", length, v->fs_type);
                return -1;
            }
            if (length < 0) {
                LOGE("format_volume: negative length (%zd) not supported on %s\n", length, v->fs_type);
                return -1;
            }
            char *num_sectors;
            if (asprintf(&num_sectors, "%zd", length / 512) <= 0) {
                LOGE("format_volume: failed to create %s command for %s\n", v->fs_type, v->blk_device);
                return -1;
            }
            const char *f2fs_path = "/sbin/mkfs.f2fs";
            const char* const f2fs_argv[] = {"mkfs.f2fs", "-t", "-d1", v->blk_device, num_sectors, NULL};

            result = exec_cmd(f2fs_path, (char* const*)f2fs_argv);
            free(num_sectors);
        }
        if (result != 0) {
            LOGE("format_volume: make %s failed on %s with %d(%s)\n", v->fs_type, v->blk_device, result, strerror(errno));
            return -1;
        }
        return 0;
    }


    if (strcmp(v->fs_type, "emmc") == 0) {
        LOGI("format_volume for: fs_type:%s blk_device:%s mount_point:%s \n", v->fs_type, v->blk_device, v->mount_point);
        int fd = open(v->blk_device, O_WRONLY);
        if (fd < 0){
            LOGE("format_volume: failed to open %s\n", v->blk_device);
            return -1;
        }
        uint64_t len = get_block_device_size(fd);
        LOGI("format_volume:len:%lld \n",len);
        if(wipe_block_device(fd,len) == 0 ) {
            LOGI("format_volume: success to format %s\n", v->blk_device);
            return 0;
        }
        else{
            LOGE("format_volume: fail to format %s\n", v->blk_device);
            return -1;
        }
    }

    LOGE("format_volume: fs_type \"%s\" unsupported\n", v->fs_type);
    return -1;
}

int format_volume(const char* volume) {
    return format_volume(volume, NULL);
}

int setup_install_mounts() {
    if (fstab == NULL) {
        LOGE("can't set up install mounts: no fstab loaded\n");
        return -1;
    }
    for (int i = 0; i < fstab->num_entries; ++i) {
        Volume* v = fstab->recs + i;

        if (strcmp(v->mount_point, "/tmp") == 0 ||
            strcmp(v->mount_point, "/cache") == 0) {
            if (ensure_path_mounted(v->mount_point) != 0) {
                LOGE("failed to mount %s\n", v->mount_point);
                return -1;
            }

        } else {
            if (strcmp(v->mount_point, "/mnt/external_sd") ==0 ||
                strcmp(v->mount_point, "/mnt/usb_storage") == 0 ||
                strcmp(v->mount_point, "/backup") ==0 ) {
                LOGE("setup_install_mounts, expect %s\n", v->mount_point);
                continue;
            }
            if (ensure_path_unmounted(v->mount_point) != 0) {
                LOGE("failed to unmount %s\n", v->mount_point);
                return -1;
            }
        }
    }
    return 0;
}
