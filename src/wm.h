#ifndef WM_H_
#define WM_H_

#include <list>

#include "geom.h"
#include "pixmap.h"

class Window;

bool init_window_manager();
void destroy_window_manager(); 

class WindowManager {
private:
	std::list<Window*> windows;

	std::list<Rect> dirty_rects;

	int bg_color[3];
	int frame_thickness;
	int titlebar_thickness;
	int frame_fcolor[3];
	int frame_ucolor[3];

	Window *root_win;
	Window *focused_win;
	Window *grab_win;

	Pixmap mouse_cursor;
	Pixmap *background;

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
	Window *get_root_window() const;

	void set_focused_frame_color(int r, int g, int b);
	void get_focused_frame_color(int *r, int *g, int *b) const;

	void set_unfocused_frame_color(int r, int g, int b);
	void get_unfocused_frame_color(int *r, int *g, int *b) const;

	void set_background(const Pixmap *pixmap);
	const Pixmap *get_background() const;

	Window *get_grab_window() const;

	void grab_mouse(Window *win);
	void release_mouse();

	void raise_window(Window *win);
	void sink_window(Window *win);

	void maximize_window(Window *win);
	void unmaximize_window(Window *win);
};

extern WindowManager *wm;

#endif	// WM_H_
