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
}

Window::~Window()
{
	delete [] title;
}

const Rect &Window::get_rect() const
{
	return rect;
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

void Window::draw()
{
	callbacks.display(this);
	dirty = false;
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
