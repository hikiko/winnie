#ifndef WM_H_
#define WM_H_

#include <list>

#include "geom.h"
#include "pixmap.h"

class Window;

class WindowManager {
private:
	std::list<Window*> windows;
	std::list<Window*> frames;

	std::list<Rect> dirty_rects;

	int bg_color[3];
	int frame_thickness;
	int titlebar_thickness;
	int frame_fcolor[3];
	int frame_ucolor[3];

	Window *root_win;
	Window *focused_win;

	Pixmap mouse_cursor;

	void create_frame(Window *win);
	void destroy_frame(Window *win);

public:
	WindowManager();
	~WindowManager();

	void invalidate_region(const Rect &rect);
	void process_windows();

	void add_window(Window *win);
	void remove_window(Window *win);

	void set_focused_window(Window *win);
	const Window *get_focused_window() const;
	Window *get_focused_window();

	Window *get_window_at_pos(int pointer_x, int pointer_y);
};

extern WindowManager *wm;

#endif	// WM_H_
