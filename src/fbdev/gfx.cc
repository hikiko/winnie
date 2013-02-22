#ifdef WINNIE_FBDEV
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
static int color_depth; // bits per pixel

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
	Rect drect = rect;

	if(drect.x < 0) {
		drect.x = 0;
	}

	if(drect.y < 0) {
		drect.y = 0;
	}

	unsigned char *fb = framebuffer + (drect.x + screen_rect.width * drect.y) * 4;
	for(int i=0; i<drect.height; i++) {
		for(int j=0; j<drect.width; j++) {
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

void blit(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img,
		const Rect &dest_rect, int dest_x, int dest_y)
{
	int width = src_rect.width;
	int height = src_rect.height;

	int xoffs = dest_x - dest_rect.x;
	if(xoffs < 0) {
		dest_x = dest_rect.x;
		width += xoffs;
	}

	int yoffs = dest_y - dest_rect.y;
	if(yoffs < 0) {
		dest_y = dest_rect.y;
		height += yoffs;
	}

	int xend = dest_x + width;
	if(xend >= dest_rect.width) {
		width -= xend - dest_rect.width;
	}

	int yend = dest_y + height;
	if(yend >= dest_rect.height) {
		height -= yend - dest_rect.height;
	}

	if(width <= 0 || height <= 0) {
		return;
	}

	unsigned char *sptr = src_img + (src_rect.y * src_rect.width + src_rect.x) * 4;
	unsigned char *dptr = dest_img + (dest_y * dest_rect.width + dest_x) * 4;

	for(int i=0; i<height; i++) {
		memcpy(dptr, sptr, width * 4);
		sptr += src_rect.width * 4;
		dptr += dest_rect.width * 4;
	}
}

void blit_key(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img,
		const Rect &dest_rect, int dest_x, int dest_y, int key_r, int key_g, int key_b)
{
	int width = src_rect.width;
	int height = src_rect.height;

	int xoffs = dest_x - dest_rect.x;
	if(xoffs < 0) {
		dest_x = dest_rect.x;
		width += xoffs;
	}

	int yoffs = dest_y - dest_rect.y;
	if(yoffs < 0) {
		dest_y = dest_rect.y;
		height += yoffs;
	}

	int xend = dest_x + width;
	if(xend >= dest_rect.width) {
		width -= xend - dest_rect.width;
	}

	int yend = dest_y + height;
	if(yend >= dest_rect.height) {
		height -= yend - dest_rect.height;
	}

	if(width <= 0 || height <= 0) {
		return;
	}

	unsigned char *sptr = src_img + (src_rect.y * src_rect.width + src_rect.x) * 4;
	unsigned char *dptr = dest_img + (dest_y * dest_rect.width + dest_x) * 4;

	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {
			int r = sptr[j * 4];
			int g = sptr[j * 4 + 1];
			int b = sptr[j * 4 + 2];

			if(r != key_r || g != key_g || b != key_b) {
				dptr[j * 4] = b;
				dptr[j * 4 + 1] = g;
				dptr[j * 4 + 2] = r;
			}
		}

		sptr += src_rect.width * 4;
		dptr += dest_rect.width * 4;
	}
}

void gfx_update()
{
}

#endif // WINNIE_FBDEV
