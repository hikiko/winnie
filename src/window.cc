#include <algorithm>
#include <string.h>

#include "gfx.h"
#include "window.h"
#include "wm.h"

Window::Window()
{
	parent = 0;
	title = 0;
	rect.x = rect.y = 0;
	rect.width = rect.height = 128;
	memset(&callbacks, 0, sizeof callbacks);
	dirty = true;
	managed = true;
	focusable = true;
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

Rect Window::get_absolute_rect() const
{
	if(!parent) {
		return rect;
	}

	Rect absolute_rect;
	absolute_rect = parent->get_absolute_rect();

	absolute_rect.x += rect.x;
	absolute_rect.y += rect.y;
	absolute_rect.width = rect.width;
	absolute_rect.height = rect.height;

	return absolute_rect;
}

bool Window::contains_point(int ptr_x, int ptr_y)
{
	Rect abs_rect = get_absolute_rect();
	return ptr_x >= abs_rect.x && ptr_x < abs_rect.x + abs_rect.width &&
			ptr_y >= abs_rect.y && ptr_y < abs_rect.y + abs_rect.height;
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
	Rect abs_rect = get_absolute_rect();
	wm->invalidate_region(abs_rect);
}

void Window::draw(const Rect &dirty_region)
{
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
	Rect abs_rect = get_absolute_rect();
	return fb + get_color_depth() * (get_screen_size().x * abs_rect.y + abs_rect.x) / 8;
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

void Window::set_focusable(bool focusable)
{
	this->focusable = focusable;
}

bool Window::get_focusable() const
{
	return focusable;
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

Window **Window::get_children()
{
	if(children.empty()) {
		return 0;
	}
	return &children[0];
}

int Window::get_children_count() const
{
	return (int)children.size();
}

const Window *Window::get_parent() const
{
	return parent;
}

Window *Window::get_parent()
{
	return parent;
}
