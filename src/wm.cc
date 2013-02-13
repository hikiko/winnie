#include <stdexcept>
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
}

void WindowManager::invalidate_region(const Rect &rect)
{
	dirty_rects.push_back(rect);
}

void WindowManager::process_windows()
{
	//TODO:
	//sta dirty rectangles na brw to union
	//na eleg3w poia einai sto dirty area k na ta kanw dirty
	//na ka8arizw ta dirty areas
	//prwta render to bg
	//meta ola ta dirty:
	std::list<Window*>::iterator it = windows.begin();
	while(it != windows.end()) {
		if((*it)->dirty) {
			(*it)->draw();
		}
	}
}
