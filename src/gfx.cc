#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <linux/fb.h>

#include "gfx.h"

#define FRAMEBUFFER_SIZE(xsz, ysz, bpp) ((xsz) * (ysz) * (bpp) / CHAR_BIT)

static unsigned char *framebuffer;
static int dev_fd = -1;

static Rect screen_rect;
static int color_depth; //bits per pixel

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

	int sz = FRAMEBUFFER_SIZE(screen_rect.width, screen_rect.height, color_depth);
	framebuffer = (unsigned char*)mmap(0, sz, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0);

	if(framebuffer == (void*)-1) {
		close(dev_fd);
		dev_fd = -1;
		fprintf(stderr, "Cannot map the framebuffer to memory : %s\n", strerror(errno));
		return false;
	}

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
}

unsigned char *get_framebuffer()
{
	return framebuffer;
}

Rect get_screen_size()
{
	return screen_rect;
}

int get_color_depth()
{
	return color_depth;
}

void clear_screen(int r, int g, int b)
{
	fill_rect(screen_rect, r, g, b);
}

void fill_rect(const Rect &rect, int r, int g, int b)
{
	unsigned char *fb = framebuffer + (rect.x + screen_rect.width * rect.y) * 4;
	for(int i=0; i<rect.height; i++) {
		for(int j=0; j<rect.width; j++) {
			fb[j * 4] = b;
			fb[j * 4 + 1] = g;
			fb[j * 4 + 2] = r;
		}
		fb += screen_rect.width * 4;
	}
}

void set_cursor_visibility(bool visible)
{
	fb_cursor curs;
	curs.enable = visible ? 1 : 0;

	if(ioctl(dev_fd, FBIO_CURSOR, &curs) == -1) {
		fprintf(stderr, "Cannot toggle cursor visibility : %s\n", strerror(errno));
	}
}

void blit(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img, int dest_x, int dest_y)
{
	Rect dest_rect;

	if(dest_x < screen_rect.x) {
		dest_rect.x = screen_rect.x;
	}

	if(dest_y < screen_rect.y) {
		dest_rect.y = screen_rect.y;
	}

	//TODO :p zzz
}
