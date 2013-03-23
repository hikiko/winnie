/*
winnie - an experimental window system

Copyright (C) 2013 Eleni Maria Stea

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#ifdef WINNIE_FBDEV
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include <linux/fb.h>

#include "gfx.h"
#include "shalloc.h"

#define FRAMEBUFFER_SIZE(xsz, ysz, bpp) ((xsz) * (ysz) * (bpp) / CHAR_BIT)

static unsigned char *framebuffer;
static int dev_fd;
static int rgb_order[3];

struct Graphics {
	Rect screen_rect;
	Rect clipping_rect;
	int color_depth;
	Pixmap *pixmap;
};

static Graphics *gfx;

bool init_gfx()
{
	if(!(gfx = (Graphics*)sh_malloc(sizeof *gfx))) {
		return false;
	}

	dev_fd = -1;

	if((dev_fd = open("/dev/fb0", O_RDWR)) == -1) {
		fprintf(stderr, "Cannot open /dev/fb0 : %s\n", strerror(errno));
		return false;
	}

	fb_var_screeninfo sinfo;
	if(ioctl(dev_fd, FBIOGET_VSCREENINFO, &sinfo) == -1) {
		close(dev_fd);
		dev_fd = -1;
		fprintf(stderr, "Unable to get screen info : %s\n", strerror(errno));
		return false;
	}

	printf("width : %d height : %d\n : bpp : %d\n", sinfo.xres, sinfo.yres, sinfo.bits_per_pixel);
	printf("virtual w: %d virtual h: %d\n", sinfo.xres_virtual, sinfo.yres_virtual);

	gfx->screen_rect.x = gfx->screen_rect.y = 0;
	gfx->screen_rect.width = sinfo.xres_virtual;
	gfx->screen_rect.height = sinfo.yres_virtual;
	gfx->color_depth = sinfo.bits_per_pixel;

	rgb_order[0] = sinfo.red.offset / 8;
	rgb_order[1] = sinfo.green.offset / 8;
	rgb_order[2] = sinfo.blue.offset / 8;

	set_clipping_rect(gfx->screen_rect);

	int sz = FRAMEBUFFER_SIZE(gfx->screen_rect.width, gfx->screen_rect.height, gfx->color_depth);
	framebuffer = (unsigned char*)mmap(0, sz, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0);

	if(framebuffer == (void*)-1) {
		close(dev_fd);
		dev_fd = -1;
		fprintf(stderr, "Cannot map the framebuffer to memory : %s\n", strerror(errno));
		return false;
	}

// TODO: uncomment when I find how to use intelfb instead of i915 GRRRR.-	
	fb_vblank vblank;
	if(ioctl(dev_fd, FBIOGET_VBLANK, &vblank) == -1) {
//		fprintf(stderr, "FBIOGET_VBLANK error: %s\n", strerror(errno));
	}
/*	
 	else {
		printf("flags: %x\n", vblank.flags);
		printf("count: %d\n", vblank.count);
		printf("beam position: %d, %d\n", vblank.hcount, vblank.vcount);
	}
*/

	if(!(gfx->pixmap = (Pixmap*)sh_malloc(sizeof(Pixmap)))) {
		fprintf(stderr, "Failed to allocate pixmap.\n");
		return false;
	}

	gfx->pixmap->width = gfx->screen_rect.width;
	gfx->pixmap->height = gfx->screen_rect.height;

	int fbsize = gfx->pixmap->width * gfx->pixmap->height * gfx->color_depth / 8;
	if(!(gfx->pixmap->pixels = (unsigned char*)sh_malloc(fbsize))) {
		fprintf(stderr, "failed to allocate the pixmap framebuffer.\n");
		return false;
	}

	return true;
}

void destroy_gfx()
{
	clear_screen(0, 0, 0);
	gfx_update(gfx->screen_rect);

	if(dev_fd != -1) {
		close(dev_fd);
	}

	dev_fd = -1;

	munmap(framebuffer, FRAMEBUFFER_SIZE(gfx->screen_rect.width, gfx->screen_rect.height, gfx->color_depth));
	framebuffer = 0;

	sh_free(gfx->pixmap->pixels);
	gfx->pixmap->pixels = 0;
	sh_free(gfx->pixmap);
	sh_free(gfx);
}

unsigned char *get_framebuffer()
{
	return gfx->pixmap->pixels;
}

Pixmap *get_framebuffer_pixmap()
{
	return gfx->pixmap;
}

Rect get_screen_size()
{
	return gfx->screen_rect;
}

int get_color_depth()
{
	return gfx->color_depth;
}

void set_clipping_rect(const Rect &rect)
{
	gfx->clipping_rect = rect_intersection(rect, get_screen_size());
}

const Rect &get_clipping_rect()
{
	return gfx->clipping_rect;
}

void set_cursor_visibility(bool visible)
{
	fb_cursor curs;
	curs.enable = visible ? 1 : 0;

	if(ioctl(dev_fd, FBIO_CURSOR, &curs) == -1) {
		fprintf(stderr, "Cannot toggle cursor visibility : %s\n", strerror(errno));
	}
}

void gfx_update(const Rect &upd_rect)
{
	Rect rect = rect_intersection(upd_rect, gfx->screen_rect);
	unsigned char *sptr = gfx->pixmap->pixels + (rect.y * gfx->screen_rect.width + rect.x) * 4;
	unsigned char *dptr = framebuffer + (rect.y * gfx->screen_rect.width + rect.x) * 4;

	for(int i=0; i<rect.height; i++) {
		memcpy(dptr, sptr, rect.width * 4);
		sptr += gfx->screen_rect.width * 4;
		dptr += gfx->screen_rect.width * 4;
	}
}

void wait_vsync()
{
	unsigned long arg = 0;
	if(ioctl(dev_fd, FBIO_WAITFORVSYNC, &arg) == -1) {
//		printf("ioctl error %s\n", strerror(errno));
	}
}

void get_rgb_order(int *r, int *g, int *b)
{
	*r = rgb_order[0];
	*g = rgb_order[1];
	*b = rgb_order[2];
}

#endif // WINNIE_FBDEV
