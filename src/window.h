#ifndef WINDOW_H_
#define WINDOW_H_

#include "geom.h"
#include "event.h"

class Window {
private:
	char *title;
	Rect rect;
	Callbacks callbacks;

	bool dirty;

public:
	Window();
	~Window();

	const Rect &get_rect() const;

	void move(int x, int y);
	void resize(int x, int y);

	void set_title(const char *s);
	const char *get_title() const;

	/* mark this window as dirty, and notify the window manager
	 * to repaint it, and anything it used to cover.
	 */
	void invalidate();

	void draw();

	unsigned char *get_win_start_on_fb();
	int get_scanline_width();

	void set_display_callback(DisplayFuncType func);
	void set_keyboard_callback(KeyboardFuncType func);
	void set_mouse_button_callback(MouseButtonFuncType func);
	void set_mouse_motion_callback(MouseMotionFuncType func);

	// XXX remove if not needed
	friend class WindowManager;
};

#endif	// WINDOW_H_
