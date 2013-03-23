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

#include <string.h>

#include "geom.h"
#include "gfx.h"

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

	int width = src_rect.width;
	int height = src_rect.height;

	int xoffs = dest_x - irect.x;
	if(xoffs < 0) {
		dest_x = irect.x;
		width += xoffs;
	}

	int yoffs = dest_y - irect.y;
	if(yoffs < 0) {
		dest_y = irect.y;
		height += yoffs;
	}

	int xend = dest_x + width;
	if(xend >= irect.width) {
		width -= xend - irect.width;
	}

	int yend = dest_y + height;
	if(yend >= irect.height) {
		height -= yend - irect.height;
	}

	if(width <= 0 || height <= 0) {
		return;
	}

	unsigned char *sptr = src_img + (src_rect.y * src_rect.width + src_rect.x) * 4;
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

	int width = src_rect.width;
	int height = src_rect.height;

	int xoffs = dest_x - irect.x;
	if(xoffs < 0) {
		dest_x = irect.x;
		width += xoffs;
	}

	int yoffs = dest_y - irect.y;
	if(yoffs < 0) {
		dest_y = irect.y;
		height += yoffs;
	}

	int xend = dest_x + width;
	if(xend >= irect.width) {
		width -= xend - irect.width;
	}

	int yend = dest_y + height;
	if(yend >= irect.height) {
		height -= yend - irect.height;
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
				dptr[j * 4 + red_offs] = r;
				dptr[j * 4 + green_offs] = g;
				dptr[j * 4 + blue_offs] = b;
			}
		}

		sptr += src_rect.width * 4;
		dptr += dest_rect.width * 4;
	}
}
