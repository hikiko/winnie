#include <stdexcept>
#include "gfx.h"
#include "wm.h"
#include "window.h"
#include "mouse.h"
#include "mouse_cursor.h"

WindowManager *wm;
static WindowManager wminst;

WindowManager::WindowManager()
{
	if(!wm) {
		wm = this;
	} else {
		throw std::runtime_error("Trying to create a second instance of WindowManager!\n");
	}

	focused_win = 0;

	bg_color[0] = 210;
	bg_color[1] = 106;
	bg_color[2] = 106;

	mouse_cursor.set_image(mouse_cursor_width, mouse_cursor_height);
	unsigned char *pixels = mouse_cursor.get_image();

	for(int i=0; i<mouse_cursor_height; i++) {
		for(int j=0; j<mouse_cursor_width; j++) {
			int val = mouse_cursor_bw[i * mouse_cursor_width + j];
			*pixels++ = val;
			*pixels++ = val;
			*pixels++ = val;
			*pixels++ = 255;
		}
	}
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
		it++;
	}

	// draw mouse cursor
	int mouse_x, mouse_y;
	get_pointer_pos(&mouse_x, &mouse_y);

	blit_key(mouse_cursor.get_image(), mouse_cursor.get_rect(),
			get_framebuffer(), get_screen_size(), mouse_x, mouse_y,
			0, 0, 0);

	Rect mouse_rect = {mouse_x, mouse_y, mouse_cursor.get_width(), mouse_cursor.get_height()};
	invalidate_region(mouse_rect);
}

void WindowManager::add_window(Window *win)
{
	if(windows.empty()) {
		focused_win = win;
	}

	windows.push_back(win);
}

void WindowManager::set_focused_window(Window *win)
{
	focused_win = win;
}

const Window *WindowManager::get_focused_window() const
{
	return focused_win;
}

Window *WindowManager::get_focused_window()
{
	return focused_win;
}

Window *WindowManager::get_window_at_pos(int pointer_x, int pointer_y)
{
	Window *win = 0;
	std::list<Window*>::reverse_iterator rit = windows.rbegin();
	while(rit != windows.rend()) {
		if((*rit)->contains_point(pointer_x, pointer_y)) {
			win = *rit;
			break;
		}
		rit++;
	}

	return win;
}
