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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "winnie.h"
#include "tunneltex.h"

static void resize(int xsz, int ysz);
static void display(Window *win);
static void timer(Window *win);
static bool calc_tunnel_mapping();

static int xsz = 512;
static int ysz = 384;

static Pixmap *tex;
static float *umap, *vmap;
static unsigned long start_time;

extern "C" bool init()
{
	Window *win = new Window;
	win->set_title("tunnel effect");
	win->move(380, 280);
	win->resize(xsz, ysz);
	win->set_display_callback(display);
	win->set_timer_callback(timer);

	tex = new Pixmap;
	tex->set_image(gimp_image.width, gimp_image.height);

	for(int i=0; i<tex->width * tex->height; i++) {
		tex->pixels[i * 4] = gimp_image.pixel_data[i * 3];
		tex->pixels[i * 4 + 1] = gimp_image.pixel_data[i * 3 + 1];
		tex->pixels[i * 4 + 2] = gimp_image.pixel_data[i * 3 + 2];
	}

	resize(xsz, ysz);
	wm->add_window(win);

	start_time = winnie_get_time();
	set_window_timer(win, 25, TIMER_REPEAT);
	return true;
}

extern "C" void cleanup()
{
	delete tex;
}

static void resize(int width, int height)
{
	xsz = width;
	ysz = height;

	calc_tunnel_mapping();
}

static void display(Window *win)
{
	Pixmap *fb = get_framebuffer_pixmap();
	Rect wrect = win->get_absolute_rect();

	if(wrect.width != xsz || wrect.height != ysz) {
		resize(wrect.width, wrect.height);
	}

	unsigned long msec = winnie_get_time() - start_time;
	float sec = (float)msec / 1000.0f;

	float uoffs = sec * 0.75;
	float voffs = sec;

	unsigned char *fbptr = fb->pixels + (wrect.y * fb->width + wrect.x) * 4;
	float *uptr = umap;
	float *vptr = vmap;

	for(int i=0; i<ysz; i++) {
		for(int j=0; j<xsz; j++) {
			float u = *uptr++ * 8.0f;
			float v = *vptr++;
			//unsigned long tx = ((((unsigned long)((u << 3) - uoffs)) & 0xff) * tex->width) >> 8;
			//unsigned long tx = ((((u << 3) * tex->width) >> 8) + uoffs) % tex->width;
			//unsigned long ty = (((unsigned long)(v + voffs * 2) & 0xff) * tex->height) >> 8;
			int tx = fmod(u - uoffs, 1.0f) * tex->width;
			if(tx < 0) {
				tx += tex->width;
			}
			int ty = fmod(v + voffs, 1.0f) * tex->height;

			float fog = 1.0f - v / 8.0f;
			if(fog < 0.0f) fog = 0.0f;
			if(fog > 1.0f) fog = 1.0f;

			unsigned char red, green, blue;
			if(i == ysz / 2 && j == xsz / 2) {
				red = green = blue = 0;
			} else {
				int offs = (ty * tex->width + tx) * 4;
				red = tex->pixels[offs];
				green = tex->pixels[offs + 1];
				blue = tex->pixels[offs + 2];
			}

			red = red * fog;
			green = green * fog;
			blue = blue * fog;

			fbptr[j * 4] = red;
			fbptr[j * 4 + 1] = green;
			fbptr[j * 4 + 2] = blue;
		}
		fbptr += fb->width * 4;
	}
}

static void timer(Window *win)
{
	wm->invalidate_region(win->get_absolute_rect());
}

static bool calc_tunnel_mapping()
{
	int i, j;
	float *uptr, *vptr;

	if(umap || vmap) {
		free(umap);
		free(vmap);
	}

	if(!(umap = (float*)malloc(xsz * ysz * sizeof *umap))) {
		return false;
	}
	if(!(vmap = (float*)malloc(xsz * ysz * sizeof *umap))) {
		free(umap);
		return false;
	}

	uptr = umap;
	vptr = vmap;

	for(i=0; i<ysz; i++) {
		for(j=0; j<xsz; j++) {
			float x = ((float)j / xsz * 2.0 - 1.0) * 1.33333;
			float y = (float)i / ysz * 2.0 - 1.0;
			float angle, z, dist;

			if(fabs(x) > 0.00001) {
				angle = atan2(y, x) + M_PI;
			} else {
				angle = y < 0.0 ? M_PI / 2.0 : 3.0 * M_PI / 2.0;
			}
			dist = sqrt(x * x + y * y);
			z = 2.0 / dist;

			*uptr++ = angle * 0.5 / M_PI;
			*vptr++ = z;
		}
	}
	return true;
}
