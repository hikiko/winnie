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
#include <time.h>
#include <math.h>

#include "winnie.h"
#include "clockface.h"

static void display(Window *win);
static void keyboard(Window *win, int key, bool pressed);
static void button(Window *win, int bn, bool pressed, int x, int y);
static void motion(Window *win, int x, int y);
static void timer(Window *win);

static Pixmap *clockface;

extern "C" bool init()
{
	Window *clock_win = new Window;
	clock_win->set_title("clock");
	clock_win->move(200, 100);
	clock_win->resize(200, 200);
	clock_win->set_display_callback(display);
	clock_win->set_keyboard_callback(keyboard);
	clock_win->set_mouse_button_callback(button);
	clock_win->set_mouse_motion_callback(motion);
	clock_win->set_timer_callback(timer);

	clockface = new Pixmap;
	clockface->set_image(gimp_clockface.width, gimp_clockface.height);

	for(int i=0; i<clockface->width * clockface->height; i++) {
		clockface->pixels[i * 4] = gimp_clockface.pixel_data[i * 3];
		clockface->pixels[i * 4 + 1] = gimp_clockface.pixel_data[i * 3 + 1];
		clockface->pixels[i * 4 + 2] = gimp_clockface.pixel_data[i * 3 + 2];
	}

	wm->add_window(clock_win);

	set_window_timer(clock_win, 1000, TIMER_REPEAT);
	return true;
}

extern "C" void cleanup()
{
	delete clockface;
}

static void draw_hand(Pixmap *pixmap, int cx, int cy, float angle, float length, float width, int r, int g, int b)
{
	float varr_in[8] = {
		0, width / 2.0f,
		-width / 2.0f, 0,
		0, -length,
		width / 2.0f, 0
	};
	int varr_out[8];

	float matrix[2][2] = {
		{(float)cos(angle), (float)-sin(angle)},
		{(float)sin(angle), (float)cos(angle)}
	};

	for(int i=0; i<4; i++) {
		float x = varr_in[i * 2];
		float y = varr_in[i * 2 + 1];
		varr_out[i * 2] = matrix[0][0] * x + matrix[0][1] * y + cx;
		varr_out[i * 2 + 1] = matrix[1][0] * x + matrix[1][1] * y + cy;
	}

	draw_polygon(pixmap, varr_out, 0, 4, r, g, b);
}

static void display(Window *win)
{
	time_t tm_sec = time(0);
	struct tm *tm = localtime(&tm_sec);

	Rect rect = win->get_absolute_rect();
	int cx = rect.x + rect.width / 2;
	int cy = rect.y + rect.height / 2;

	float hr_len = rect.width * 0.25;
	float min_len = rect.width * 0.4;
	float sec_len = rect.width * 0.38;

	float sec_angle = M_PI * (float)tm->tm_sec / 30.0f;
	float min_angle = M_PI * (float)tm->tm_min / 30.0f;
	float hour_angle = M_PI * (float)tm->tm_hour / 6.0f;

	Pixmap *fb = get_framebuffer_pixmap();
	blit(clockface->pixels, Rect(0, 0, clockface->width, clockface->height),
			fb->pixels, get_screen_size(), rect.x, rect.y);

	draw_hand(fb, cx, cy, hour_angle, hr_len, 16, 0, 0, 0);
	draw_hand(fb, cx, cy, min_angle, min_len, 12, 64, 64, 64);

	int x = (int)(sin(sec_angle) * sec_len);
	int y = (int)(-cos(sec_angle) * sec_len);
	draw_line(fb, cx, cy, cx + x, cy + y, 200, 64, 45);
	draw_line(fb, cx + 1, cy, cx + x + 1, cy + y, 200, 64, 45);
	draw_line(fb, cx, cy + 1, cx + x, cy + y + 1, 200, 64, 45);
}

static void keyboard(Window *win, int key, bool pressed)
{
	printf("WINDOW(%p) keyboard %d %s\n", (void*)win, key, pressed ? "press" : "release");
}

static void button(Window *win, int bn, bool pressed, int x, int y)
{
	printf("WINDOW(%p) button %d %s\n", (void*)win, bn, pressed ? "press" : "release");
}

static void motion(Window *win, int x, int y)
{
	printf("WINDOW(%p) motion %d %d\n", (void*)win, x, y);
}

static void timer(Window *win)
{
	wm->invalidate_region(win->get_absolute_rect());
}
