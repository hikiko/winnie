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
#include <string.h>
#include <inttypes.h>
#include <algorithm>

#include "geom.h"
#include "gfx.h"

// used by the polygon rasterizer
#define MAX_SCANLINES	2048
static int32_t left_fx[MAX_SCANLINES], right_fx[MAX_SCANLINES];


void clear_screen(int r, int g, int b)
{
	Rect screen_rect = get_screen_size();
	fill_rect(screen_rect, r, g, b);
}

void fill_rect(const Rect &rect, int r, int g, int b)
{
	Rect drect = rect;
	Rect screen_rect = get_screen_size();
	Rect clipping_rect = get_clipping_rect();

	if(drect.x < clipping_rect.x) {
		drect.width -= clipping_rect.x - drect.x;
		drect.x = clipping_rect.x;
	}

	if(drect.y < clipping_rect.y) {
		drect.height -= clipping_rect.y - drect.y;
		drect.y = clipping_rect.y;
	}

	if(drect.x + drect.width >= clipping_rect.x + clipping_rect.width) {
		drect.width = clipping_rect.width + clipping_rect.x - drect.x;
	}

	if(drect.y + drect.height >= clipping_rect.y + clipping_rect.height) {
		drect.height = clipping_rect.height + clipping_rect.y - drect.y;
	}

	unsigned char *fb = get_framebuffer() + (drect.x + screen_rect.width * drect.y) * 4;
	for(int i=0; i<drect.height; i++) {
		for(int j=0; j<drect.width; j++) {
			fb[j * 4] = b;
			fb[j * 4 + 1] = g;
			fb[j * 4 + 2] = r;
		}
		fb += screen_rect.width * 4;
	}
}

void blit(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img,
		const Rect &dest_rect, int dest_x, int dest_y)
{
	int red_offs, green_offs, blue_offs;
	get_rgb_order(&red_offs, &green_offs, &blue_offs);

	Rect irect = rect_intersection(get_clipping_rect(), dest_rect);

	int src_xoffs = 0;
	int src_yoffs = 0;
	int width = src_rect.width;
	int height = src_rect.height;

	int xoffs = dest_x - irect.x;
	if(xoffs < 0) {
		dest_x = irect.x;
		width += xoffs;
		src_xoffs = -xoffs;
	}

	int yoffs = dest_y - irect.y;
	if(yoffs < 0) {
		dest_y = irect.y;
		height += yoffs;
		src_yoffs = -yoffs;
	}

	int xend = dest_x + width;
	if(xend >= irect.x + irect.width) {
		width -= xend - (irect.x + irect.width);
	}

	int yend = dest_y + height;
	if(yend >= irect.y + irect.height) {
		height -= yend - (irect.y + irect.height);
	}

	if(width <= 0 || height <= 0) {
		return;
	}

	unsigned char *sptr = src_img + ((src_rect.y + src_yoffs) * src_rect.width + src_rect.x + src_xoffs) * 4;
	unsigned char *dptr = dest_img + (dest_y * dest_rect.width + dest_x) * 4;

	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {
			dptr[j * 4 + red_offs] = sptr[j * 4];
			dptr[j * 4 + green_offs] = sptr[j * 4 + 1];
			dptr[j * 4 + blue_offs] = sptr[j * 4 + 2];
		}
		sptr += src_rect.width * 4;
		dptr += dest_rect.width * 4;
	}
}

void blit_key(unsigned char *src_img, const Rect &src_rect, unsigned char* dest_img,
		const Rect &dest_rect, int dest_x, int dest_y, int key_r, int key_g, int key_b)
{
	int red_offs, green_offs, blue_offs;
	get_rgb_order(&red_offs, &green_offs, &blue_offs);

	Rect irect = rect_intersection(get_clipping_rect(), dest_rect);

	int src_xoffs = 0;
	int src_yoffs = 0;
	int width = src_rect.width;
	int height = src_rect.height;

	int xoffs = dest_x - irect.x;
	if(xoffs < 0) {
		dest_x = irect.x;
		width += xoffs;
		src_xoffs = -xoffs;
	}

	int yoffs = dest_y - irect.y;
	if(yoffs < 0) {
		dest_y = irect.y;
		height += yoffs;
		src_yoffs = -yoffs;
	}

	int xend = dest_x + width;
	if(xend >= irect.x + irect.width) {
		width -= xend - (irect.x + irect.width);
	}

	int yend = dest_y + height;
	if(yend >= irect.y + irect.height) {
		height -= yend - (irect.y + irect.height);
	}

	if(width <= 0 || height <= 0) {
		return;
	}

	unsigned char *sptr = src_img + ((src_rect.y + src_yoffs) * src_rect.width + src_rect.x + src_xoffs) * 4;
	unsigned char *dptr = dest_img + (dest_y * dest_rect.width + dest_x) * 4;

	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {
			int r = sptr[j * 4];
			int g = sptr[j * 4 + 1];
			int b = sptr[j * 4 + 2];

			if(r != key_r || g != key_g || b != key_b) {
				dptr[j * 4 + red_offs] = r;
				dptr[j * 4 + green_offs] = g;
				dptr[j * 4 + blue_offs] = b;
			}
		}

		sptr += src_rect.width * 4;
		dptr += dest_rect.width * 4;
	}
}

void draw_line(Pixmap *pixmap, int x0, int y0, int x1, int y1, int r, int g, int b)
{
	int red_offs = 0, green_offs = 1, blue_offs = 2;
	if(pixmap == get_framebuffer_pixmap()) {
		get_rgb_order(&red_offs, &green_offs, &blue_offs);
	}

	unsigned char *ptr = pixmap->pixels + (y0 * pixmap->width + x0) * 4;

	int dx = x1 - x0;
	int dy = y1 - y0;

	int x_inc = 4;
	int y_inc = pixmap->width * 4;

	if(dx < 0) {
		x_inc = -x_inc;
		dx = -dx;
	}
	if(dy < 0) {
		y_inc = -y_inc;
		dy = -dy;
	}

	if(dx > dy) {
		int error = (dy << 1) - dx;
		for(int i=0; i<=dx; i++) {
			ptr[red_offs] = r;
			ptr[green_offs] = g;
			ptr[blue_offs] = b;
			if(error >= 0) {
				error -= dx << 1;
				ptr += y_inc;
			}
			error += dy << 1;
			ptr += x_inc;
		}
	} else {
		int error = (dx << 1) - dy;
		for(int i=0; i<=dy; i++) {
			ptr[red_offs] = r;
			ptr[green_offs] = g;
			ptr[blue_offs] = b;
			if(error >= 0) {
				error -= dy << 1;
				ptr += x_inc;
			}
			error += dx << 1;
			ptr += y_inc;
		}
	}
}

void draw_polygon(Pixmap *pixmap, int *vpos, int *vtex, int num_verts, int r, int g, int b)
{
	int roffs = 0, goffs = 1, boffs = 2;
	if(pixmap == get_framebuffer_pixmap()) {
		get_rgb_order(&roffs, &goffs, &boffs);
	}

	int ystart = pixmap->height, yend = 0;

	for(int i=0; i<num_verts; i++) {
		int next = (i + 1) % num_verts;
		int x0 = vpos[i * 2];
		int y0 = vpos[i * 2 + 1];
		int x1 = vpos[next * 2];
		int y1 = vpos[next * 2 + 1];

		int dx = x1 - x0;
		int dy = y1 - y0;

		if(dy == 0)
			continue;

		/* continue in 24.8 fixed point */
		int32_t fslope = ((int32_t)dx << 8) / (int32_t)abs(dy);
		int32_t fx = x0 << 8;

		if(dy >= 0) {
			for(int j=y0; j<=y1; j++) {
				right_fx[j] = fx;
				fx += fslope;
			}
		} else {
			for(int j=y0; j>=y1; j--) {
				left_fx[j] = fx;
				fx += fslope;
			}
		}

		ystart = std::min(ystart, std::min(y0, y1));
		yend = std::max(yend, std::max(y0, y1));
	}

	if(ystart < 0) ystart = 0;
	if(yend > pixmap->height) yend = pixmap->height;

	for(int i=ystart; i<yend; i++) {
		int x = (left_fx[i] + (1 << 7)) >> 8;
		int dx = (right_fx[i] - left_fx[i]) >> 8;

		// accept polygons of either order
		if(dx < 0) {
			x = (right_fx[i] + (1 << 7)) >> 8;
			dx = -dx;
		}

		if(x < 0) {
			dx += x;
			x = 0;
		}
		if(x + dx >= pixmap->width) {
			dx = pixmap->width - x;
		}
		unsigned char *pixptr = pixmap->pixels + (i * pixmap->width + x) * 4;

		for(int j=0; j<dx; j++) {
			pixptr[roffs] = r;
			pixptr[goffs] = g;
			pixptr[boffs] = b;
			pixptr += 4;
		}
	}
}
