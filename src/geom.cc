#include "geom.h"

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
