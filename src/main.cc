#include <stdio.h>
#include <stdlib.h>

#include "winnie.h"

static void display(Window *win);
static void keyboard(Window *win, int key, bool pressed);
static void cleanup();

int main()
{
	winnie_init();
	atexit(cleanup);

	Window *win1 = new Window;
	win1->set_title("title1");
	win1->move(5, 10);
	win1->resize(600, 800);
	win1->set_display_callback(display);
	win1->set_keyboard_callback(keyboard);

	wm->add_window(win1);

	while(1) {
		process_events();
	}

	winnie_shutdown();
}

static void display(Window *win)
{
	fill_rect(win->get_rect(), 0, 0, 0);
}

static void keyboard(Window *win, int key, bool pressed)
{
	switch(key) {
	case 'q':
		exit(0);
	}
}

static void cleanup()
{
	destroy_gfx();
}
