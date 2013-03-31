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
