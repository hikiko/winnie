#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pixmap.h"

Pixmap::Pixmap()
{
	width = height = 0;
	pixels = 0;
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
	Rect rect = {0, 0, width, height};
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
	return false;	// TODO
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
