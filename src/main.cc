#include <stdio.h>
#include <stdlib.h>

#include "winnie.h"

static void display(Window *win);
static void keyboard(Window *win, int key, bool pressed);
static void button(Window *win, int bn, bool pressed, int x, int y);
static void motion(Window *win, int x, int y);
static void cleanup();

int main()
{
	if(!winnie_init()) {
		exit(1);
	}

	atexit(cleanup);

	Window *win1 = new Window;
	win1->set_title("title1");
	win1->move(5, 10);
	win1->resize(200, 300);
	win1->set_display_callback(display);
	win1->set_keyboard_callback(keyboard);
	win1->set_mouse_button_callback(button);
	win1->set_mouse_motion_callback(motion);

	wm->add_window(win1);

	while(1) {
		process_events();
	}
}

static void display(Window *win)
{
	if(wm->get_focused_window() != win) {
		fill_rect(win->get_absolute_rect(), 106, 106, 250);
	}
	else {
		fill_rect(win->get_absolute_rect(), 0, 0, 255);
	}
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

static void cleanup()
{
	winnie_shutdown();
}
