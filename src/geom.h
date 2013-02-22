#ifndef GEOM_H_
#define GEOM_H_

struct Rect {
	int x, y;
	int width, height;
};

Rect rect_union(const Rect &a, const Rect &b);
Rect rect_intersection(const Rect &a, const Rect &b);

#endif	// GEOM_H_
