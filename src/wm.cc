#include <stdexcept>
#include "gfx.h"
#include "wm.h"
#include "window.h"

WindowManager *wm;
static WindowManager wminst;

WindowManager::WindowManager()
{
	if(!wm) {
		wm = this;
	} else {
		throw std::runtime_error("Trying to create a second instance of WindowManager!\n");
	}

	bg_color[0] = 210;
	bg_color[1] = 106;
	bg_color[2] = 106;
}

void WindowManager::invalidate_region(const Rect &rect)
{
	dirty_rects.push_back(rect);
}

void WindowManager::process_windows()
{
	if(dirty_rects.empty()) {
		return;
	}

	std::list<Rect>::iterator drit = dirty_rects.begin();
	Rect uni = *drit++;
	while(drit != dirty_rects.end()) {
		uni = rect_union(uni, *drit++);
	}
	dirty_rects.clear();

	fill_rect(uni, bg_color[0], bg_color[1], bg_color[2]);
	
	std::list<Window*>::iterator it = windows.begin();
	while(it != windows.end()) {
		Rect intersect = rect_intersection((*it)->rect, uni); 
		if(intersect.width && intersect.height) {
			(*it)->draw();
		}
	}
}
