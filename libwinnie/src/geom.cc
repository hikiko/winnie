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

#include "geom.h"

Rect::Rect()
{
	x = y = width = height = 0;
}

Rect::Rect(int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	width = w;
	height = h;
}

static inline int min(int x, int y)
{
	return x < y ? x : y;
}

static inline int max(int x, int y)
{
	return x > y ? x : y;
}

Rect rect_union(const Rect &a, const Rect &b)
{
	Rect uni;
	uni.x = min(a.x, b.x);
	uni.y = min(a.y, b.y);
	uni.width = max(a.x + a.width, b.x + b.width) - uni.x;
	uni.height = max(a.y + a.height, b.y + b.height) - uni.y;

	return uni;
}

Rect rect_intersection(const Rect &a, const Rect &b)
{
	Rect intersect;
	intersect.x = max(a.x, b.x);
	intersect.y = max(a.y, b.y);
	intersect.width = max(min(a.x + a.width, b.x + b.width) - intersect.x, 0);
	intersect.height = max(min(a.y + a.height, b.y + b.height) - intersect.y, 0);

	return intersect;
}
