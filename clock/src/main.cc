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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winnie.h"

static void display(Window *win);
static void keyboard(Window *win, int key, bool pressed);
static void button(Window *win, int bn, bool pressed, int x, int y);
static void motion(Window *win, int x, int y);

Subsys* subsys;

int main()
{
	if(!winnie_open()) {
		exit(1);
	}

	Window *clock_win = new Window;
	clock_win->set_title("Clipping the win title");
	clock_win->move(200, 100);
	clock_win->resize(200, 300);
	clock_win->set_display_callback(display);
	clock_win->set_keyboard_callback(keyboard);
	clock_win->set_mouse_button_callback(button);
	clock_win->set_mouse_motion_callback(motion);

	wm->add_window(clock_win);

	winnie_close();
}

static void display(Window *win)
{
	fill_rect(win->get_absolute_rect(), 128, 128, 128);
}

static void keyboard(Window *win, int key, bool pressed)
{
	switch(key) {
		case 'q':
			exit(0);
	}
}

static void button(Window *win, int bn, bool pressed, int x, int y)
{
	printf("WINDOW(%p) button %d %s\n", (void*)win, bn, pressed ? "press" : "release");
}

static void motion(Window *win, int x, int y)
{
	printf("WINDOW(%p) motion %d %d\n", (void*)win, x, y);	
}
