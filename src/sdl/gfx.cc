#ifdef WINNIE_SDL
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "gfx.h"

static SDL_Surface *fbsurf;

static Rect screen_rect = {0, 0, 1024, 768};
static int color_depth = 32; // bits per pixel

bool init_gfx()
{
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "failed to initialize SDL\n");
		return false;
	}

	if(!(fbsurf = SDL_SetVideoMode(screen_rect.width, screen_rect.height, color_depth, 0))) {
		fprintf(stderr, "failed to set video mode\n");
		return false;
	}
	SDL_ShowCursor(0);

	return true;
}

void destroy_gfx()
{
	SDL_Quit();
}

unsigned char *get_framebuffer()
{
	return (unsigned char*)fbsurf->pixels;
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
	uint32_t color = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
	
	SDL_Rect sdl_rect;
	sdl_rect.x = rect.x;
	sdl_rect.y = rect.y;
	sdl_rect.w = rect.width;
	sdl_rect.h = rect.height;

	SDL_FillRect(fbsurf, &sdl_rect, color);
}

void set_cursor_visibility(bool visible)
{
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
				dptr[j * 4] = r;
				dptr[j * 4 + 1] = g;
				dptr[j * 4 + 2] = b;
			}
		}

		sptr += src_rect.width * 4;
		dptr += dest_rect.width * 4;
	}
}

void gfx_update()
{
	SDL_UpdateRect(fbsurf, 0, 0, 0, 0);
}

#endif // WINNIE_SDL
