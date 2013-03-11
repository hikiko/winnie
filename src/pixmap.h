#ifndef PIXMAP_H_
#define PIXMAP_H_

#include "geom.h"

class Pixmap {
public:
	int width, height;
	unsigned char *pixels;

	Pixmap();

	Pixmap(const Pixmap &pixmap);
	Pixmap &operator=(const Pixmap& pixmap);

	~Pixmap();

	int get_width() const;
	int get_height() const;
	Rect get_rect() const;

	bool set_image(int x, int y, unsigned char *pix = 0);
	const unsigned char *get_image() const;
	unsigned char *get_image();

	bool load(const char *fname);
	bool save(const char *fname) const;
};

#endif	// PIXMAP_H_
