/*
winnie - an experimental window system

Copyright (C) 2013 Eleni Maria Stea

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <algorithm>
#include <limits.h>
#include <stdexcept>
#include <stdio.h>	// TODO

#include "gfx.h"
#include "mouse.h"
#include "mouse_cursor.h"
#include "text.h"
#include "wm.h"
#include "window.h"
#include "winnie.h"

#define DCLICK_INTERVAL 400

WindowManager *wm;

static void display(Window *win);
static void mouse(Window *win, int bn, bool pressed, int x, int y);
static void motion(Window *win, int x, int y);

bool init_window_manager()
{
	if(!(wm = new WindowManager)) {
		return false;
	}

	return true;
}

void destroy_window_manager()
{
	delete wm;
}

void WindowManager::create_frame(Window *win)
{
	Window *frame = new Window;
	Window *parent = win->get_parent();

	frame->set_display_callback(display);
	frame->set_mouse_button_callback(mouse);
	frame->set_mouse_motion_callback(motion);
	frame->set_focusable(false);
	frame->add_child(win);

	windows.push_back(frame);

	Rect win_rect = win->get_rect();
	frame->move(win_rect.x - frame_thickness,
			win_rect.y - frame_thickness - titlebar_thickness);
	frame->resize(win_rect.width + frame_thickness * 2,
			win_rect.height + frame_thickness * 2 + titlebar_thickness);

	win->move(frame_thickness, frame_thickness + titlebar_thickness);
	parent->add_child(frame);
}

void WindowManager::destroy_frame(Window *win)
{
	Window *frame = win->parent;
	if(!frame) {
		return;
	}

	if(grab_win == win) {
		release_mouse();
	}

	std::list<Window*>::iterator it;
	it = std::find(windows.begin(), windows.end(), frame);
	if(it != windows.end()) {
		root_win->add_child(win);
		windows.erase(it);
		delete frame;
	}
}

WindowManager::WindowManager()
{
	if(!wm) {
		wm = this;
	} else {
		throw std::runtime_error("Trying to create a second instance of WindowManager!\n");
	}

	root_win = new Window;
	root_win->resize(get_screen_size().width, get_screen_size().height);
	root_win->move(0, 0);
	root_win->set_managed(false);

	grab_win = 0;
	focused_win = 0;
	background = 0;

	bg_color[0] = 210;
	bg_color[1] = 106;
	bg_color[2] = 106;

	frame_thickness = 8;
	titlebar_thickness = 16;

	set_focused_frame_color(0, 0, 0);
	set_unfocused_frame_color(200, 200, 200);

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

WindowManager::~WindowManager()
{
	delete root_win;
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

	wait_vsync();

	if(!background) {
		fill_rect(uni, bg_color[0], bg_color[1], bg_color[2]);
	}
	else {
		blit(background->pixels, Rect(0, 0, background->width, background->height),
				get_framebuffer(), get_screen_size(), 0, 0);
	}

	root_win->draw_children(uni);

	// draw mouse cursor
	int mouse_x, mouse_y;
	get_pointer_pos(&mouse_x, &mouse_y);

	blit_key(mouse_cursor.get_image(), mouse_cursor.get_rect(),
			get_framebuffer(), get_screen_size(), mouse_x, mouse_y,
			0, 0, 0);

	Rect mouse_rect(mouse_x, mouse_y, mouse_cursor.get_width(), mouse_cursor.get_height());
	invalidate_region(mouse_rect);

	gfx_update(uni);
}

void WindowManager::add_window(Window *win)
{
	if(!win || win == root_win) {
		return;
	}

	root_win->add_child(win);

	if(windows.empty()) {
		focused_win = win;
	}

	if(win->get_managed()) {
		create_frame(win);
	}

	windows.push_back(win);
}

void WindowManager::remove_window(Window *win)
{
	std::list<Window*>::iterator it;
	it = std::find(windows.begin(), windows.end(), win);

	if(it != windows.end()) {
		windows.erase(it);
	}
}

void WindowManager::set_focused_window(Window *win)
{
	if(win && win == focused_win) {
		return;
	}

	Window *parent;
	if(focused_win) {
		// invalidate the frame (if any)
		parent = focused_win->get_parent();
		if(parent && parent != root_win) {
			parent->invalidate();
			fill_rect(parent->get_absolute_rect(), frame_ucolor[0], frame_ucolor[1], frame_ucolor[2]);
		}
	}

	if(!win) {
		focused_win = 0;
		return;
	}

	if(win->get_focusable()) {
		focused_win = win;
		parent = focused_win->get_parent();
		fill_rect(parent->get_absolute_rect(), frame_fcolor[0], frame_fcolor[1], frame_fcolor[2]);
		return;
	}

	Window **children = win->get_children();
	for(int i=0; i<win->get_children_count(); i++) {
		if(children[0]->get_focusable()) {
			set_focused_window(children[0]);
			fill_rect(win->get_absolute_rect(), frame_fcolor[0], frame_fcolor[1], frame_fcolor[2]);
			return;
		}
	}

	focused_win = 0;
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
	Window *root_win = wm->get_root_window();
	Window **children = root_win->get_children();
	for(int i=root_win->get_children_count() - 1; i>=0; i--) {
		if(children[i]->contains_point(pointer_x, pointer_y)) {
			return children[i];
		}
	}

	return 0;
}

Window *WindowManager::get_root_window() const
{
	return root_win;
}

void WindowManager::set_focused_frame_color(int r, int g, int b)
{
	frame_fcolor[0] = r;
	frame_fcolor[1] = g;
	frame_fcolor[2] = b;
}

void WindowManager::get_focused_frame_color(int *r, int *g, int *b) const
{
	*r = frame_fcolor[0];
	*g = frame_fcolor[1];
	*b = frame_fcolor[2];
}

void WindowManager::set_unfocused_frame_color(int r, int g, int b)
{
	frame_ucolor[0] = r;
	frame_ucolor[1] = g;
	frame_ucolor[2] = b;
}

void WindowManager::get_unfocused_frame_color(int *r, int *g, int *b) const
{
	*r = frame_ucolor[0];
	*g = frame_ucolor[1];
	*b = frame_ucolor[2];
}

void WindowManager::set_background(const Pixmap *pixmap)
{
	if(background) {
		delete background;
	}

	if(pixmap) {
		background = new Pixmap(*pixmap);
	}
	else {
		background = 0;
	}
}

const Pixmap *WindowManager::get_background() const
{
	return background;
}

Window *WindowManager::get_grab_window() const
{
	return grab_win;
}

void WindowManager::grab_mouse(Window *win)
{
	grab_win = win;
}

void WindowManager::release_mouse()
{
	grab_win = 0;
}

void WindowManager::raise_window(Window *win)
{
	if(!win) {
		return;
	}

	Window *parent = win->get_parent();
	if(parent != root_win) {
		if(parent->get_parent() == root_win) {
			win = parent;
		}
		else {
			return;
		}
	}

	root_win->remove_child(win);
	root_win->add_child(win);
}

void WindowManager::sink_window(Window *win)
{
	if(!win) {
		return;
	}

	std::list<Window*>::iterator it;
	it = std::find(windows.begin(), windows.end(), win);
	if(it != windows.end()) {
		windows.erase(it);
		windows.push_front(win);
	}
}

void WindowManager::maximize_window(Window *win)
{
	win->normal_rect = win->rect;
	
	Rect rect = get_screen_size();

	Window *frame;
	if((frame = win->get_parent())) {
		frame->normal_rect = frame->rect;
		frame->resize(rect.width, rect.height);
		frame->move(rect.x, rect.y);

		rect.width -= frame_thickness * 2;
		rect.height -= frame_thickness * 2 + titlebar_thickness;
	}
	else {
		win->move(0, 0);
	}

	win->resize(rect.width, rect.height);
	win->set_state(Window::STATE_MAXIMIZED);
}

void WindowManager::unmaximize_window(Window *win)
{
	win->resize(win->normal_rect.width, win->normal_rect.height);
	win->move(win->normal_rect.x, win->normal_rect.y);

	Window *frame;
	if((frame = win->get_parent())) {
		frame->resize(frame->normal_rect.width, frame->normal_rect.height);
		frame->move(frame->normal_rect.x, frame->normal_rect.y);
	}

	win->set_state(Window::STATE_NORMAL);
}

static void display(Window *win)
{
	//frame display:
	Window *child = win->get_children()[0];
	int r, g, b;
	Rect abs_rect = win->get_absolute_rect();

	//TODO 5 not hardcoded
	set_text_position(abs_rect.x + 5, abs_rect.y + 15);
	set_text_color(255, 255, 255);

	if(child == wm->get_focused_window()) {
		wm->get_focused_frame_color(&r, &g, &b);
		fill_rect(abs_rect, r, g, b);
	}
	else {
		wm->get_unfocused_frame_color(&r, &g, &b);
		fill_rect(win->get_absolute_rect(), r, g, b);
	}

	draw_text(child->get_title());
}

static int prev_x, prev_y;

static void mouse(Window *win, int bn, bool pressed, int x, int y)
{
	static long last_click = 0;

	if(bn == 0) {
		if(pressed) {	
			wm->grab_mouse(win);
			wm->raise_window(win);
			prev_x = x;
			prev_y = y;
		}
		else {
			long time = winnie_get_time();
			if((time - last_click) < DCLICK_INTERVAL) {
				Window *child = win->get_children()[0];
				Window::State state = child->get_state();
				if(state == Window::STATE_MAXIMIZED) {
					wm->unmaximize_window(child);
				}
				else if(state == Window::STATE_NORMAL) {
					wm->maximize_window(child);
				}
			}
			last_click = time;

			wm->release_mouse();
		}
	}
}

static void motion(Window *win, int x, int y)
{
	int left_bn = get_button(0);

	if(left_bn) {
		int dx = x - prev_x;
		int dy = y - prev_y;
		prev_x = x - dx;
		prev_y = y - dy;

		if(win->get_children()[0]->get_state() != Window::STATE_MAXIMIZED) {
			Rect rect = win->get_rect();
			win->move(rect.x + dx, rect.y + dy);
		}
	}
}
