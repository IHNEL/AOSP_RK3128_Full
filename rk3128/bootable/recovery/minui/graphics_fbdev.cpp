/*
 * Copyright (C) 2014 The Android Open Source Project
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>

#include <sys/cdefs.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include "minui.h"
#include "graphics.h"

static GRSurface* fbdev_init(minui_backend*);
static GRSurface* fbdev_flip(minui_backend*);
static void fbdev_blank(minui_backend*, bool);
static void fbdev_exit(minui_backend*);

static GRSurface gr_framebuffer[2];
static bool double_buffered;
static GRSurface* gr_draw = NULL;
static int displayed_buffer;

static fb_var_screeninfo vi;
static int fb_fd = -1;

static minui_backend my_backend = {
    .init = fbdev_init,
    .flip = fbdev_flip,
    .blank = fbdev_blank,
    .exit = fbdev_exit,
};

void rk_rotate_surface_90(GRSurface* surface, int width, int height){
    int byt = 4; // 4 byte for ARGB_8888 (2 byte for RGB_565)
    int draw_width = (width + 15) & (~15);
    int draw_height = (height + 15) & (~15);
    int length = draw_width * draw_height;
    unsigned char* des_data = (unsigned char *)malloc(sizeof(unsigned char)*length*byt);
    memset(des_data, 0, sizeof(unsigned char)*length*byt);
    memcpy(des_data, surface->data, sizeof(unsigned char)*length*byt);

    int i, j;
    int n1, n2;
    for(i = 0; i < draw_height; i++){
        for(j = 0; j < draw_width; j++){
            n1 = j*draw_height+draw_height-i-1;
            n2 = i*draw_width+j;
            surface->data[n1*byt] = des_data[n2*byt];
            surface->data[n1*byt+1] = des_data[n2*byt+1];
            surface->data[n1*byt+2] = des_data[n2*byt+2];
            surface->data[n1*byt+3] = des_data[n2*byt+3];
        }
    }
    free(des_data);
}

void rk_rotate_surface_180(GRSurface* surface)
{
    printf("(%s:%d) --- start.\n", __func__, __LINE__);
    int draw_width = (surface->width + 15) & (~15);
    int draw_height = (surface->height + 15) & (~15);
    int byt = 4; // 4 byte for ARGB_8888 (2 byte for RGB_565)

    int length = draw_width * draw_height;
    unsigned char * des_data = (unsigned char *)malloc(sizeof(unsigned char)*length*byt);
    memcpy(des_data,surface->data,sizeof(unsigned char)*length*byt);

    memset(surface->data, 0, sizeof(unsigned char)*length*byt);
    int i = 0;
    for (i=0; i<length; i++)
    {
        surface->data[i*byt] = des_data[(length-i-1)*byt];
        surface->data[i*byt+1] = des_data[(length-i-1)*byt+1];
        surface->data[i*byt+2] = des_data[(length-i-1)*byt+2];
        surface->data[i*byt+3] = des_data[(length-i-1)*byt+3];
    }

    free(des_data);
}

void rk_rotate_surface_270(GRSurface* surface, int width, int height){
    int byt = 4; // 4 byte for ARGB_8888 (2 byte for RGB_565)
    int draw_width = (width + 15) & (~15);
    int draw_height = (height + 15) & (~15);
    int length = draw_width * draw_height;
    unsigned char* des_data = (unsigned char *)malloc(sizeof(unsigned char)*length*byt);
    memcpy(des_data, surface->data, sizeof(unsigned char)*length*byt);
    memset(surface->data, 0, sizeof(unsigned char)*length*byt);
    int i, j;
    int n1, n2;
    for(i = 0; i < draw_height; i++){
        for(j = 0; j < draw_width; j++){
            n1 = (draw_width-j-1)*draw_height+i;
            n2 = i*draw_width+j;
            surface->data[n1*byt] = des_data[n2*byt];
            surface->data[n1*byt+1] = des_data[n2*byt+1];
            surface->data[n1*byt+2] = des_data[n2*byt+2];
            surface->data[n1*byt+3] = des_data[n2*byt+3];
        }
    }

    free(des_data);
}

minui_backend* open_fbdev() {
    return &my_backend;
}

static void fbdev_blank(minui_backend* backend __unused, bool blank)
{
    int ret;

    ret = ioctl(fb_fd, FBIOBLANK, blank ? FB_BLANK_POWERDOWN : FB_BLANK_UNBLANK);
    if (ret < 0)
        perror("ioctl(): blank");
}

static void set_displayed_framebuffer(unsigned n)
{
    if (n > 1 || !double_buffered) return;

    vi.yres_virtual = gr_framebuffer[0].height * 2;
    vi.yoffset = n * gr_framebuffer[0].height;
    vi.bits_per_pixel = gr_framebuffer[0].pixel_bytes * 8;
#ifdef PLATFORM_PRODUCT_BOX
    vi.grayscale = 0;
#endif

    if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vi) < 0) {
        perror("active fb swap failed");
    }
    displayed_buffer = n;
}

static GRSurface* fbdev_init(minui_backend* backend) {
    int fd = open("/dev/graphics/fb0", O_RDWR);
    if (fd == -1) {
        perror("cannot open fb0");
        return NULL;
    }

    fb_fix_screeninfo fi;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fi) < 0) {
        perror("failed to get fb0 info");
        close(fd);
        return NULL;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
        perror("failed to get fb0 info");
        close(fd);
        return NULL;
    }

    // We print this out for informational purposes only, but
    // throughout we assume that the framebuffer device uses an RGBX
    // pixel format.  This is the case for every development device I
    // have access to.  For some of those devices (eg, hammerhead aka
    // Nexus 5), FBIOGET_VSCREENINFO *reports* that it wants a
    // different format (XBGR) but actually produces the correct
    // results on the display when you write RGBX.
    //
    // If you have a device that actually *needs* another pixel format
    // (ie, BGRX, or 565), patches welcome...

    printf("fb0 reports (possibly inaccurate):\n"
           "  vi.bits_per_pixel = %d\n"
           "  vi.red.offset   = %3d   .length = %3d\n"
           "  vi.green.offset = %3d   .length = %3d\n"
           "  vi.blue.offset  = %3d   .length = %3d\n"
           "  fi.line_length = %d\n",
           vi.bits_per_pixel,
           vi.red.offset, vi.red.length,
           vi.green.offset, vi.green.length,
           vi.blue.offset, vi.blue.length,
                  fi.line_length);

    //GGL_PIXEL_FORMAT_RGBX_8888
    vi.red.offset     = 0;
    vi.red.length     = 8;
    vi.green.offset   = 8;
    vi.green.length   = 8;
    vi.blue.offset    = 16;
    vi.blue.length    = 8;
    vi.transp.offset  = 24;
    vi.transp.length  = 8;
    vi.bits_per_pixel = 32;
    vi.nonstd = 2;

    void* bits = mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bits == MAP_FAILED) {
        perror("failed to mmap framebuffer");
        close(fd);
        return NULL;
    }

    memset(bits, 0, fi.smem_len);

    gr_framebuffer[0].width = vi.xres;
    gr_framebuffer[0].height = vi.yres;
    gr_framebuffer[0].row_bytes = fi.line_length;
    gr_framebuffer[0].row_bytes = vi.xres * 4;

    gr_framebuffer[0].pixel_bytes = vi.bits_per_pixel / 8;
    gr_framebuffer[0].data = reinterpret_cast<uint8_t*>(bits);
    memset(gr_framebuffer[0].data, 0, gr_framebuffer[0].height * gr_framebuffer[0].row_bytes);

    /* check if we can use double buffering */
    if (vi.yres * fi.line_length * 2 <= fi.smem_len) {
        double_buffered = true;

        memcpy(gr_framebuffer+1, gr_framebuffer, sizeof(GRSurface));
        gr_framebuffer[1].data = gr_framebuffer[0].data +
            gr_framebuffer[0].height * gr_framebuffer[0].row_bytes;

        gr_draw = gr_framebuffer+1;

    } else {
        double_buffered = false;

        // Without double-buffering, we allocate RAM for a buffer to
        // draw in, and then "flipping" the buffer consists of a
        // memcpy from the buffer we allocated to the framebuffer.

        gr_draw = (GRSurface*) malloc(sizeof(GRSurface));
        memcpy(gr_draw, gr_framebuffer, sizeof(GRSurface));
        gr_draw->data = (unsigned char*) malloc(gr_draw->height * gr_draw->row_bytes);
        if (!gr_draw->data) {
            perror("failed to allocate in-memory surface");
            return NULL;
        }
    }

    memset(gr_draw->data, 0, gr_draw->height * gr_draw->row_bytes);
    fb_fd = fd;
    set_displayed_framebuffer(0);

    printf("framebuffer: %d (%d x %d)\n", fb_fd, gr_draw->width, gr_draw->height);

    //fbdev_blank(backend, true);
    fbdev_blank(backend, false);

    return gr_draw;
}

static GRSurface* fbdev_flip(minui_backend* backend __unused) {
    if (double_buffered) {
        // Change gr_draw to point to the buffer currently displayed,
        // then flip the driver so we're displaying the other buffer
        // instead.
        gr_draw = gr_framebuffer + displayed_buffer;
#ifdef RotateScreen_90
        rk_rotate_surface_90(&gr_framebuffer[1-displayed_buffer], gr_framebuffer[1-displayed_buffer].height, gr_framebuffer[1-displayed_buffer].width);
#elif defined RotateScreen_180
        rk_rotate_surface_180(&gr_framebuffer[1-displayed_buffer]);
#elif defined RotateScreen_270
        rk_rotate_surface_270(&gr_framebuffer[1-displayed_buffer], gr_framebuffer[1-displayed_buffer].height, gr_framebuffer[1-displayed_buffer].width);
#endif
        set_displayed_framebuffer(1-displayed_buffer);
    } else {
        // Copy from the in-memory surface to the framebuffer.
        memcpy(gr_framebuffer[0].data, gr_draw->data,
               gr_draw->height * gr_draw->row_bytes);
    }
    return gr_draw;
}

static void fbdev_exit(minui_backend* backend __unused) {
    close(fb_fd);
    fb_fd = -1;

    if (!double_buffered && gr_draw) {
        free(gr_draw->data);
        free(gr_draw);
    }
    gr_draw = NULL;
}
