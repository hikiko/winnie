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
#include <errno.h>
#include "pixmap.h"

Pixmap::Pixmap()
{
	width = height = 0;
	pixels = 0;
}

Pixmap::Pixmap(const Pixmap &pixmap)
{
	width = height = 0;
	pixels = 0;
	set_image(pixmap.width, pixmap.height, pixmap.pixels);
}

Pixmap &Pixmap::operator=(const Pixmap &pixmap)
{
	if(this != &pixmap) {
		set_image(pixmap.width, pixmap.height, pixmap.pixels);
	}

	return *this;
}

Pixmap::~Pixmap()
{
	if(pixels) {
		delete [] pixels;
	}
}

int Pixmap::get_width() const
{
	return width;
}

int Pixmap::get_height() const
{
	return height;
}

Rect Pixmap::get_rect() const
{
	Rect rect(0, 0, width, height);
	return rect;
}

bool Pixmap::set_image(int x, int y, unsigned char *pix)
{
	delete [] pixels;

	pixels = new unsigned char[x * y * 4];
	width = x;
	height = y;

	if(pix) {
		memcpy(pixels, pix, x * y * 4);
	}
	return true;
}

const unsigned char *Pixmap::get_image() const
{
	return pixels;
}

unsigned char *Pixmap::get_image()
{
	return pixels;
}

bool Pixmap::load(const char *fname)
{
	FILE *fp;
	int hdrline = 0;

	if(!(fp = fopen(fname, "rb"))) {
		fprintf(stderr, "failed to open pixmap: %s: %s\n", fname, strerror(errno));
		return false;
	}

	/* read ppm header */
	while(hdrline < 3) {
		char buf[64];

		if(!fgets(buf, sizeof buf, fp))
			goto err;
		
		/* skip comments */
		if(buf[0] == '#')
			continue;

		switch(hdrline++) {
		case 0:
			/* first header line should be P6 */
			if(strcmp(buf, "P6\n") != 0)
				goto err;
			break;

		case 1:
			/* second header line contains the pixmap dimensions */
			if(sscanf(buf, "%d %d", &width, &height) != 2)
				goto err;
			break;
		}
	}

	set_image(width, height, 0);

	for(int i=0; i<width * height * 4; i++) {
		int c;
		if(i % 4 != 3) {
			c = fgetc(fp);
			if(c < 0)
				goto err;
		}
		else {
			c = 255;
		}
		pixels[i] = c;
	}
	fclose(fp);
	return true;

err:
	fprintf(stderr, "failed to load pixmap: %s\n", fname);
	fclose(fp);
	return false;
}

bool Pixmap::save(const char *fname) const
{
	if(!pixels) {
		return false;
	}

	FILE *fp = fopen(fname, "wb");
	if(!fp) {
		fprintf(stderr, "failed to save pixmap: %s: %s\n", fname, strerror(errno));
		return false;
	}

	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	for(int i=0; i<width * height; i++) {
		fputc(pixels[i * 4], fp);
		fputc(pixels[i * 4 + 1], fp);
		fputc(pixels[i * 4 + 2], fp);
	}

	fclose(fp);
	return true;
}
