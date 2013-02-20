#include <algorithm>
#include <string.h>

#include "gfx.h"
#include "window.h"
#include "wm.h"

Window::Window()
{
	title = 0;
	rect.x = rect.y = 0;
	rect.width = rect.height = 128;
	memset(&callbacks, 0, sizeof callbacks);
	dirty = true;
	managed = true;
}

Window::~Window()
{
	for(size_t i=0; i<children.size(); i++) {
		wm->remove_window(children[i]);
		delete children[i];
	}

	delete [] title;
}

const Rect &Window::get_rect() const
{
	return rect;
}

bool Window::contains_point(int ptr_x, int ptr_y)
{
	return ptr_x >= rect.x && ptr_x < rect.x + rect.width &&
			ptr_y >= rect.y && ptr_y < rect.y + rect.height;
}

void Window::move(int x, int y)
{
	invalidate();	// moved, should redraw, MUST BE CALLED FIRST
	rect.x = x;
	rect.y = y;
}

void Window::resize(int x, int y)
{
	invalidate();	// resized, should redraw, MUST BE CALLED FIRST
	rect.width = x;
	rect.height = y;
}

void Window::set_title(const char *s)
{
	delete [] title;

	title = new char[strlen(s) + 1];
	strcpy(title, s);
}

const char *Window::get_title() const
{
	return title;
}

void Window::invalidate()
{
	dirty = true;
	wm->invalidate_region(rect);
}

void Window::draw(const Rect &dirty_region)
{
	//TODO
	//titlebar, frame

	Rect intersect = rect_intersection(rect, dirty_region);
	if(intersect.width && intersect.height) {
		if(callbacks.display) {
			callbacks.display(this);
		}
		dirty = false;

		draw_children(rect);
	}
}

void Window::draw_children(const Rect &dirty_region)
{
	for(size_t i=0; i<children.size(); i++) {
		children[i]->draw(dirty_region);
	}
}

unsigned char *Window::get_win_start_on_fb()
{
	unsigned char *fb = get_framebuffer();
	return fb + get_color_depth() * (get_screen_size().x * rect.y + rect.x) / 8;
}

int Window::get_scanline_width()
{
	return get_screen_size().x;
}

void Window::set_managed(bool managed)
{
	this->managed = managed;
}

bool Window::get_managed() const
{
	return managed;
}

void Window::set_display_callback(DisplayFuncType func)
{
	callbacks.display = func;
}

void Window::set_keyboard_callback(KeyboardFuncType func)
{
	callbacks.keyboard = func;
}

void Window::set_mouse_button_callback(MouseButtonFuncType func)
{
	callbacks.button = func;
}

void Window::set_mouse_motion_callback(MouseMotionFuncType func)
{
	callbacks.motion = func;
}

const DisplayFuncType Window::get_display_callback() const
{
	return callbacks.display;
}

const KeyboardFuncType Window::get_keyboard_callback() const
{
	return callbacks.keyboard;
}

const MouseButtonFuncType Window::get_mouse_button_callback() const
{
	return callbacks.button;
}

const MouseMotionFuncType Window::get_mouse_motion_callback() const
{
	return callbacks.motion;
}

void Window::add_child(Window *win)
{
	children.push_back(win);
	if(win->parent) {
		win->parent->remove_child(win);
	}
	win->parent = this;
}

void Window::remove_child(Window *win)
{
	std::vector<Window*>::iterator it;
	it = std::find(children.begin(), children.end(), win);
	if(it != children.end()) {
		children.erase(it);
		win->parent = 0;
	}
}

const Window *Window::get_parent() const
{
	return parent;
}

Window *Window::get_parent()
{
	return parent;
}
