#ifdef WINNIE_FBDEV
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include <linux/fb.h>

#include "gfx.h"

#define FRAMEBUFFER_SIZE(xsz, ysz, bpp) ((xsz) * (ysz) * (bpp) / CHAR_BIT)

static unsigned char *framebuffer;
static int dev_fd = -1;

static Rect screen_rect;

static int color_depth; // bits per pixel

static Pixmap *pixmap;

bool init_gfx()
{
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

	screen_rect.x = screen_rect.y = 0;
	screen_rect.width = sinfo.xres_virtual;
	screen_rect.height = sinfo.yres_virtual;
	color_depth = sinfo.bits_per_pixel;

	set_clipping_rect(screen_rect);

	int sz = FRAMEBUFFER_SIZE(screen_rect.width, screen_rect.height, color_depth);
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

	pixmap = new Pixmap;
	pixmap->width = screen_rect.width;
	pixmap->height = screen_rect.height;
	pixmap->pixels = framebuffer;

	return true;
}

void destroy_gfx()
{
	clear_screen(0, 0, 0);

	if(dev_fd != -1) {
		close(dev_fd);
	}

	dev_fd = -1;

	munmap(framebuffer, FRAMEBUFFER_SIZE(screen_rect.width, screen_rect.height, color_depth));
	framebuffer = 0;

	pixmap->pixels = 0;
}

unsigned char *get_framebuffer()
{
	return framebuffer;
}

Pixmap *get_framebuffer_pixmap()
{
	return pixmap;
}

Rect get_screen_size()
{
	return screen_rect;
}

int get_color_depth()
{
	return color_depth;
}


void set_cursor_visibility(bool visible)
{
	fb_cursor curs;
	curs.enable = visible ? 1 : 0;

	if(ioctl(dev_fd, FBIO_CURSOR, &curs) == -1) {
		fprintf(stderr, "Cannot toggle cursor visibility : %s\n", strerror(errno));
	}
}

void gfx_update()
{
}

void wait_vsync()
{
	unsigned long arg = 0;
	if(ioctl(dev_fd, FBIO_WAITFORVSYNC, &arg) == -1) {
//		printf("ioctl error %s\n", strerror(errno));
	}
}

#endif // WINNIE_FBDEV
