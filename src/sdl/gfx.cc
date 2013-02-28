#ifdef WINNIE_SDL
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "gfx.h"

static SDL_Surface *fbsurf;

static Rect screen_rect = {0, 0, 1024, 768};
static Rect clipping_rect;

static int color_depth = 32; // bits per pixel

static Pixmap *pixmap;

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

	pixmap = new Pixmap;
	pixmap->width = screen_rect.width;
	pixmap->height = screen_rect.height;
	pixmap->pixels = (unsigned char*)fbsurf->pixels;

	set_clipping_rect(screen_rect);

	return true;
}

void destroy_gfx()
{
	pixmap->pixels = 0;
	delete pixmap;
	SDL_Quit();
}

unsigned char *get_framebuffer()
{
	return (unsigned char*)fbsurf->pixels;
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

/*void set_clipping_rect(const Rect &rect)
{
	clipping_rect = rect_intersection(rect, screen_rect);

	SDL_Rect sdl_rect;
	sdl_rect.x = clipping_rect.x;
	sdl_rect.y = clipping_rect.y;
	sdl_rect.w = clipping_rect.width;
	sdl_rect.h = clipping_rect.height;

	SDL_SetClipRect(fbsurf, &sdl_rect);
}

const Rect &get_clipping_rect()
{
	return clipping_rect;
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
}*/

void set_cursor_visibility(bool visible)
{
}

void gfx_update()
{
	SDL_UpdateRect(fbsurf, 0, 0, 0, 0);
}

void wait_vsync()
{
}

#endif // WINNIE_SDL
