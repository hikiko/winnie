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

#ifdef WINNIE_SDL
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "gfx.h"
#include "shalloc.h"
#include "winnie.h"

static SDL_Surface *fbsurf;

struct Graphics {
	Rect screen_rect;
	Rect clipping_rect;
	int color_depth; // bits per pixel
	Pixmap *pixmap;
};

static Graphics *gfx;

bool init_gfx()
{
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "failed to initialize SDL\n");
		return false;
	}

	if(!(gfx = (Graphics*)sh_malloc(sizeof *gfx))) {
		return false;
	}

	get_subsys()->graphics_offset = (int)((char*)gfx - (char*)get_pool());

	Rect scr_rect(0, 0, 1024, 768);
	gfx->screen_rect = scr_rect;
	gfx->color_depth = 32;

	if(!(fbsurf = SDL_SetVideoMode(gfx->screen_rect.width, gfx->screen_rect.height, gfx->color_depth, 0))) {
		fprintf(stderr, "Failed to set video mode\n");
		return false;
	}
	SDL_ShowCursor(0);

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

	set_clipping_rect(gfx->screen_rect);

	return true;
}

void destroy_gfx()
{
	sh_free(gfx->pixmap->pixels);
	gfx->pixmap->pixels = 0;
	sh_free(gfx->pixmap);
	sh_free(gfx);
	SDL_Quit();
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
}

void gfx_update(const Rect &upd_rect)
{
	if(SDL_MUSTLOCK(fbsurf)) {
		SDL_LockSurface(fbsurf);
	}

	Rect rect = rect_intersection(upd_rect, gfx->screen_rect);

	unsigned char *sptr = gfx->pixmap->pixels + (rect.y * gfx->screen_rect.width + rect.x) * 4;
	unsigned char *dptr = (unsigned char*)fbsurf->pixels + (rect.y * gfx->screen_rect.width + rect.x) * 4;

	for(int i=0; i<rect.height; i++) {
		memcpy(dptr, sptr, rect.width * 4);
		sptr += gfx->screen_rect.width * 4;
		dptr += gfx->screen_rect.width * 4;
	}

	if(SDL_MUSTLOCK(fbsurf)) {
		SDL_UnlockSurface(fbsurf);
	}
	SDL_UpdateRect(fbsurf, rect.x, rect.y, rect.width, rect.height);
}

void wait_vsync()
{
}

void get_rgb_order(int *r, int *g, int *b)
{
	*r = fbsurf->format->Rshift / 8;
	*g = fbsurf->format->Gshift / 8;
	*b = fbsurf->format->Bshift / 8;
}

#endif // WINNIE_SDL
