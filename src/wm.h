#ifndef WM_H_
#define WM_H_

#include <list>
#include "geom.h"

class Window;

class WindowManager {
private:
	std::list<Window*> windows;
	std::list<Rect> dirty_rects;

public:
	WindowManager();

	void invalidate_region(const Rect &rect);
	void process_windows();
};

extern WindowManager *wm;

#endif	// WM_H_
